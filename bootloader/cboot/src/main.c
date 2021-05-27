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
#include <md5.h>
#include <bootinfo.h>

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
    BiosMemoryRegion* memmap = (BiosMemoryRegion*)BIOS_MEMORY_MAP_LOCATION;

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

void* allocate_space_for_kernel(usize mem_size){
    const int MINBASE = 0x100000;       // make sure we load the kernel above the 1MB area

    BiosMemoryRegion* memmap = (BiosMemoryRegion*)BIOS_MEMORY_MAP_LOCATION;

    while(memmap->type != 0){

        if(memmap->base_address < MINBASE){
            memmap++;
            continue;
        } else if (memmap->map_size < mem_size){
            memmap++;
            continue;
        } else if (memmap->type == BIOS_MAP_FREE_MEMORY){
            // + it's above MINBASE
            // + it's at least mem_size
            // + and it's free memory
            // translate it to a virtual address and return a void pointer to it
            return physical_to_virtual_pointer((void*)memmap->base_address);
        }

        memmap++;
    }

    return 0;
}

void* map_kernel(u8* elf_file, u8* code_buffer, u64* kernel_size){
    ELF64FileHeader* elf_header = (ELF64FileHeader*)elf_file;

    u8* phdr_it = elf_file + elf_header->program_header_table_position;

    // Program header counter
    usize phdr_counter = 0;
    puts("Program Headers:\n");
    puts("  Type           Offset             VirtAddr           PhysAddr\n");
    puts("                 FileSiz            MemSiz              Flags  Align\n");
    do {
        ELF64ProgramHeader* elf_phdr = (ELF64ProgramHeader*)phdr_it;

        if(elf_phdr->type == 1){
            //This is a load segment - we need to load it

            puts("  LOAD           0x");
            putx64(elf_phdr->offset);
            puts(" 0x");
            putx64(elf_phdr->vaddr);
            puts(" 0x");
            putx64(elf_phdr->paddr);
            puts("\n                 0x");
            putx64(elf_phdr->filesz);
            puts(" 0x");
            putx64(elf_phdr->memsz);
            puts("         0x");
            putx16((u16)elf_phdr->align);
            putc('\n');

            //copy the segment data into the code buffer
            memcpy(code_buffer+elf_phdr->offset-elf_phdr->align, elf_file + elf_phdr->offset, elf_phdr->filesz);
            
            // calculate the number of pages we are going to need to map this data
            usize num_pages = ALIGN_UP_4KB(elf_phdr->memsz)/0x1000;

            //map the data in the code buffer to the specified vaddr
            for(usize i = 0; i < num_pages; i++){
                VirtAddr vaddr;
                vaddr.raw = elf_phdr->vaddr + 0x1000*i;
                map_4kb_page(vaddr, (u64)code_buffer+elf_phdr->offset-elf_phdr->align & (~0xFFFFE00000000FFF));
            }

            *kernel_size += elf_phdr->memsz;
        }

        phdr_it += elf_header->program_header_table_entry_size;
        phdr_counter++;
    } while(phdr_counter < elf_header->program_header_table_entry_count);

    return (void*)elf_header->entry_point;
}

void cboot_main() {
    setup_interrupts();
    init_serial();

    BootInfo bootinfo;
    memset(&bootinfo, 0, sizeof(bootinfo));

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

    // Find a place in memory to load the kernel file (kaddr is a virtual address starting with 0xFFFF8...)
    // Allocate 256mb
    u8* kaddr = (u8*)allocate_space_for_kernel(268435456);
    memset(kaddr, 0, 268435456);

    if(kaddr == 0){
        puts("Not Enough Memory to Load Kernel\n");
        halt();
    }
    //else

    // we will use the top 96mb to load the elf file - and the bottom 160mb will hold the loaded executable data
    u8* exe = kaddr;
    kaddr = exe + 167772160;


    puts("Loading kernel\n");

    u64 bytes_loaded = ustar_load_file(gpte, "/sys/kernel.elf", kaddr);

    if(bytes_loaded==0){
        puts("Error loading kernel file");
        halt();
    }
    //else

    puts("Loaded Kernel\n");

    //check the elf signature
    int neq = memcmp(kaddr, "\x7F\x45\x4c\x46", 4);

    if(neq){
        puts("ERR: ELF signature not matching\n");
        halt();
    }
    //else 

    /*
    // calculate the md5sum of the kernel.elf file
    MD5_CTX ctx; u8 md5sum[16];
    MD5_Init(&ctx);
    MD5_Update(&ctx, kaddr, bytes_loaded);
    MD5_Final(md5sum,&ctx);
    
    // kernel image
    puts("md5sum(kernel.elf) = ");
    hexdump(md5sum, 16);
    putc('\n');
    */

    void* entry_point = map_kernel(kaddr, exe, &bootinfo.kernel_size);

    /*
    puts("entry point = ");
    putx64((u64)entry_point);
    putc('\n');

    puts("hexdump at entry point: ");
    hexdump((u8*)entry_point, 32);
    putc('\n');
    */

    puts("KERNEL MAPPED, JUMPING TO KERNEL\n");

    bootinfo.bios_memory_map_address = (BiosMemoryRegion*)0x500;
    bootinfo.kernel_paddr = (u64)exe & ~0xFFFFE00000000000;

    // Jump to the kernel
    void(*kmain)(BootInfo*) = (void (*)(BootInfo*))entry_point;
    
    kmain(&bootinfo);

    halt();
}


void halt(){
    puts("Halting");
    while(1) {
        __asm__("hlt");
    }
}