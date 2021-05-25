#include <int_types.h>
#include <vga.h>
#include <ata.h>
#include <gpt.h>
#include <mem.h>
#include <bios_memory_map.h>

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

void print_bios_memmap(){
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
            puts(" - FREE MEMORY");
            total_memory += memmap->map_size;
            break;
        case BIOS_MAP_RESERVED:
            puts(" - RESERVED");
            break;
        case BIOS_MAP_ACPI_RECLAIM:
            puts(" - ACPI RECLAIM");
            break;
        case BIOS_MAP_ACPI_NVS:
            puts(" - ACPI NVS");
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

}

void cboot_main() {

    puts("Hello, World from the bootloader\n");
  
    puts("Bios Physical Memory Maps\n");
    print_bios_memmap();
    putc('\n');

    GPTEntry gpte = find_partition_by_name("OSDEV Root Partition");

    if(gpte.starting_lba != 0 && gpte.ending_lba != 0){
        puts("Partition Found, Time to load the operating system\n");
    }

    puts("\nHalting\n");
    while(1) {
        __asm__("hlt");
    }
}
