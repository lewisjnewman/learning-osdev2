#include <int_types.h>
#include <vga.h>
#include <ata.h>

int check_read_sectors(){
    u8 buf[512];

    // Read the first sector off of the disk into the buffer
    ATA_PIO_read_sectors(&buf, 0, 1);

    if (buf[510] != 0x55){
        return 1;
    }
    if (buf[511] != 0xAA){
        return 1;
    }
    return 0;
}

void cboot_main() {

    puts("Hello, World from the bootloader\n");

    int check = check_read_sectors();
    if(check){
        puts("Error reading mbr from disk\n");
    } else {
        puts("Success reading mbr from disk\n");
    }

    while(1) {}
}
