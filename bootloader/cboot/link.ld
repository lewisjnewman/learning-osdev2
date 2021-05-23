ENTRY(_start)
OUTPUT_FORMAT("binary")
SECTIONS {
    . = 0x70000;
    .text :
    {
        *(.boot)
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
    }
    .data :
    {
        *(.data)
    }
    .bss :
    {
        *(.bss)
    }
}