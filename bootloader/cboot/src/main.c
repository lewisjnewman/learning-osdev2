#include <int_types.h>
#include <vga.h>
#include <ata.h>
#include <gpt.h>
#include <mem.h>
#include <bios_memory_map.h>
#include <ustar.h>
#include <paging.h>
#include <interrupts.h>
#include <elf64.h>
#include <print.h>
#include <serial.h>

void halt();

GPTEntry find_partition_by_name(char* name){

    u8 buf[4096];
    GPTHeader* gpt = (GPTHeader*)(buf);

    GPTEntry gpte_found;
    memset(&gpte_found, 0, sizeof(GPTEntry));

    ATA_PIO_read_sectors(&buf, 1, 1);

    puts("GPT Signature: ");
    for(usize i = 0; i < 8; i++){
        putc(gpt->signature[i]);
    }
    putc('\n');

    usize gpte_size = gpt->sizeof_partition_entry;

    // This loads lba 2..9 inclusive for a total of 32 partition entries - it is unlikely we are booting on a disk with that many partitions on it
    ATA_PIO_read_sectors(&buf, 2, 8);

    u8* gpte_ptr = (u8*)(buf);
    GPTEntry* gpte = (GPTEntry*)gpte_ptr;

    u8 nullbuf[16];
    memset(nullbuf, 0, 16);

    int found = 0;

    puts("Looking For Partition: ");
    puts(name);
    putc('\n');

    // While we have not reached a null partition entry
    while(memcmp(nullbuf, &gpte->partition_type_guid, 16)){
        char part_name[33];
        memset(part_name, 0, 33);
        
        for(usize i = 0; i < 32; i++){
            part_name[i] = gpte->partition_name[i];
        }


        puts("Partition Detected: ");
        puts(part_name);
        putc('\n');

        int result = strncmp(name, part_name, 32);

        if(result == 0 && found == 0){
            // dereference to return a copy of the gpte pointer
            gpte_found = *gpte;
            found = 1;

            puts("System Partition Found :)\n");
        }

        // We make sure to increment by the size specified by the sizeof_partition_entry
        // field in the GPT header, otherwise we could endup reading the wrong slice of memory
        gpte_ptr += gpte_size;
        gpte = (GPTEntry*)gpte_ptr;
    }

    return gpte_found;
}

u64 scan_bios_memmap(){
    bios_memmap* memmap = (bios_memmap*)BIOS_MEMORY_MAP_LOCATION;

    u64 total_memory = 0;

    while(memmap->type != 0){

        puts("0x");
        putx64(memmap->base_address);
        puts(" - 0x");
        putx64(memmap->base_address + memmap->map_size);

        switch (memmap->type)
        {
        case BIOS_MAP_FREE_MEMORY:
            puts(" - FREE MEMORY (1)");
            total_memory += memmap->map_size;
            break;
        case BIOS_MAP_RESERVED:
            puts(" - RESERVED (2)");
            break;
        case BIOS_MAP_ACPI_RECLAIM:
            puts(" - ACPI RECLAIM (3)");
            break;
        case BIOS_MAP_ACPI_NVS:
            puts(" - ACPI NVS (4)");
            break;
        case BIOS_BAD_MEMORY:
            puts(" - BAD MEMORY (5)");
            break;
        default:
            break;
        }

        putc('\n');
        memmap++;
    }

    puts("Total Size of Usable Memory: ");
    putd(total_memory);
    putc('\n');

    return total_memory;
}

void* allocate_space_for_kernel(){
    const int MINBASE = 0x100000;       // make sure we load the kernel above the 1MB area
    const int PHYSMEM_SIZE = 100663296; // make sure the area we allocate is at least 96MB in size

    bios_memmap* memmap = (bios_memmap*)BIOS_MEMORY_MAP_LOCATION;

    while(memmap->type != 0){

        if(memmap->base_address < MINBASE){
            memmap++;
            continue;
        } else if (memmap->map_size < PHYSMEM_SIZE){
            memmap++;
            continue;
        } else if (memmap->type == BIOS_MAP_FREE_MEMORY){
            // + it's above MINBASE
            // + it's at least PHYSMEM_SIZE
            // + and it's free memory
            // translate it to a virtual address and return a void pointer to it
            return physical_to_virtual_pointer((void*)memmap->base_address);
        }

        puts("0x");
        putx64(memmap->base_address);
        puts(" - 0x");
        putx64(memmap->base_address + memmap->map_size);

        switch (memmap->type)
        {
        case BIOS_MAP_FREE_MEMORY:
            puts(" - FREE MEMORY (1)");
            break;
        default:
            break;
        }

        putc('\n');
        memmap++;
    }

    return 0;
}


void* map_kernel(ELF64FileHeader* kernelelf, u64 elfsize){
    //TODO - fix this up, there are a lot of assumptions about layout of the elf file in this section

    u8* byte_ptr = (u8*)kernelelf;

    // get a pointer to the area in memory after the elf file
    // aligned up to the next page boundry
    u8* afterfile = (u8*)((u64)(byte_ptr+elfsize+4096) & ~0x1FFF);

    // Work out where the first program header is
    byte_ptr += kernelelf->program_header_table_position;
    ELF64ProgramHeader* ph = (ELF64ProgramHeader*)(byte_ptr);

    // For storing the pointers to and sizes of the segments (there should definitely be less than 64)
    u8* segment_ptrs[64];
    u64 segment_sizes[64];

    u64 num_segments = 0;

    // Iterate through the first 3 program headers (in theory this should be .text, .data, .rodata)
    puts("PROGRAM HEADERS\n");
    for(usize i = 0; i < kernelelf->program_header_table_entry_count; i++){
        if(ph->segment_type != 1){
            // Ignore this segment if it's not a load segment
            continue;
        }

        // here we copy the segment data into the area above where the elf file is loaded
        memcpy(afterfile, byte_ptr+ph->offset, ph->segment_size_file);

        // save a pointer to where the segment data is and it's size for later
        segment_ptrs[num_segments] = afterfile;
        segment_sizes[num_segments] = ALIGN_UP_4KB(ph->segment_size_memory);

        // move the afterfile pointer to new free memory
        afterfile += ALIGN_UP_4KB(ph->segment_size_memory);

        // move the program header pointer forward
        byte_ptr += kernelelf->program_header_table_entry_size;
        ph = (ELF64ProgramHeader*)byte_ptr;

        num_segments++;
    }
    puts("PROGRAM HEADERS END\n");

    void* entry_point = (void*)kernelelf->entry_point;

    u64 total_size = 0;

    // Copy the segment data back down overwriting the elf file header data
    for(usize i = 0; i < num_segments; i++){
        memcpy(byte_ptr, segment_ptrs[i], segment_sizes[i]);
        byte_ptr += segment_sizes[i];
        total_size += segment_sizes[i];
    }

    u64 paddr = (u64)kernelelf & 0x00007FFFFFFFFFFF;

    // setup paging to map the kernel executable area at 0xFFFF900000000000
    VirtAddr vaddr;
    vaddr.raw = 0xFFFF900000000000;
    for(u64 i = 0; i < total_size; i += 0x1000){
        puts("vaddr = ");
        putx64(vaddr.raw);
        puts(" paddr = ");
        putx64(paddr);
        putc('\n');

        map_4kb_page(vaddr, paddr);

        vaddr.raw += 0x1000;
        paddr += 0x1000;
    }

    // return the entry point
    return entry_point;
}

void cboot_main() {
    setup_interrupts();
    init_serial();

    puts("Hello, World from the bootloader\n");
  
    puts("Bios Physical Memory Maps\n");
    u64 total_mem = scan_bios_memmap();

    GPTEntry gpte = find_partition_by_name("OSDEV Root Partition");

    if(gpte.starting_lba != 0 && gpte.ending_lba != 0){
        puts("Partition Found, Time to load the operating system\n");
    }

    //ustar_list_files(gpte);

    setup_physical_map(total_mem);

    u64 kernel_filesize = 0;
    usize isfound = ustar_get_num_sectors(gpte, "/sys/kernel.elf", &kernel_filesize);
    kernel_filesize *= 512;

    if(isfound != 0){
        puts("Unable To File the kernel executable file\n");
    }
    else {
        puts("Kernel Size On Disk = ");
        putd(kernel_filesize);
        putc('\n');
    }

    // Find a place in memory to load the kernel file (kaddr is a virtual address in 0xFFFF8...)
    void* kaddr = allocate_space_for_kernel();

    if(kaddr == 0){
        puts("Not Enough Memory to Load Kernel\n");
        halt();
    }
    //else

    puts("Loading kernel\n");

    u64 bytes_loaded = ustar_load_file(gpte, "/sys/kernel.elf", (u8*)kaddr);

    if(bytes_loaded==0){
        puts("Error loading kernel file");
        halt();
    }
    //else

    puts("Loaded Kernel\n");

    ELF64FileHeader* kernelelf = (ELF64FileHeader*)kaddr;

    //check the elf signature
    int neq = memcmp(kernelelf, "\x7F\x45\x4c\x46", 4);

    if(neq){
        puts("ERR: ELF signature not matching\n");
        halt();
    }
    //else 



    // map the kernel sections into the appropriate areas
    void* entry_point = map_kernel(kernelelf, kernel_filesize);

    // Jump to the kernel
    void (*kmain)(void) = (void (*)())entry_point;
    kmain();

    halt();
}


void halt(){
    puts("Halting");
    while(1) {
        __asm__("hlt");
    }
}