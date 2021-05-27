ENTRY(kmain)
OUTPUT_FORMAT(elf64-x86-64)
SECTIONS {
    . = 0xFFFF900000000000;
    .text : {
        . = ALIGN(4096);
        *(.text)
    }
    .rodata : {
        . = ALIGN(4096);
        *(.rodata)
    }
    .data : {
        . = ALIGN(4096);
        *(.data)
    }
    .bss : {
        . = ALIGN(4096);
        *(.bss)
    }
}