ENTRY(_start)
OUTPUT_FORMAT("binary")
SECTIONS {
    . = 0x10000;
    .text :
    {
        *(.boot)
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