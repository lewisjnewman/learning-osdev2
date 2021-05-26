ENTRY(kmain)
OUTPUT_FORMAT(elf64-x86-64)
SECTIONS {
    . = 0xFFFF900000000000;
    .text :
    {
        *(.text)
        . = ALIGN(4096);
    }
    .rodata :
    {
        *(.rodata)
        . = ALIGN(4096);
    }
    .data :
    {
        *(.data)
        . = ALIGN(4096);
    }
    .bss :
    {
        *(.bss)
    }
}