#include <stdint.h>


void puts(const char *string) {
    volatile char* vidmem = (volatile char*)0xB8000;

    while(*string != '\0'){
        *vidmem++ = *string++;
        *vidmem++ = 0x0F;
    } 

}

void cboot_main() {

    puts("Hello, World From the 64bit bootloader!");

    //return to the asm - this causes the bootloader to halt
}
