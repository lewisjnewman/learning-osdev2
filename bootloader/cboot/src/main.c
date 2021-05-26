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
            return virtual_to_physical_pointer((void*)memmap->base_address);
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

void cboot_main() {
    setup_interrupts();

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

    puts("Loaded Kernel To ");
    putx64((u64)kaddr);
    putc('\n');
    puts("Loaded ");
    putd(bytes_loaded);
    puts(" bytes\n");

    ELF64FileHeader* kernelelf = (ELF64FileHeader*)kaddr;
    putx8(kernelelf->magic[0]);
    putc(' ');
    putc(kernelelf->magic[1]);
    putc(kernelelf->magic[2]);
    putc(kernelelf->magic[3]);

    putc('\n');

    halt();
}


void halt(){
    puts("Halting");
    while(1) {
        __asm__("hlt");
    }
}