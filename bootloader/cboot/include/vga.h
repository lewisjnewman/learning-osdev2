#ifndef VGA_H
#define VGA_H

#include <int_types.h>

typedef enum{
    BLACK = 0x0,
    BLUE = 0x1,
    GREEN = 0x2,
    CYAN = 0x3,
    RED = 0x4,
    MAGENTA = 0x5,
    BROWN = 0x6,
    LIGHT_GRAY = 0x7,
    DARK_GRAY = 0x8,
    LIGHT_BLUE = 0x9,
    LIGHT_GREEN = 0x10,
    LIGHT_CYAN = 0x11,
    LIGHT_RED = 0x12,
    LIGHT_MAGENTA = 0x13,
    YELLOW = 0x14,
    WHITE = 0x15,
} BiosColours;

extern void clear_screen();
extern void set_foreground(BiosColours colour);
extern void set_background(BiosColours colour);
extern void putc(char c);
extern void puts(char* s);

//functions for printing out integers to the screen in hex
extern void putx8(u8 i);
extern void putx16(u16 i);
extern void putx32(u32 i);
extern void putx64(u64 i);

// for printing out integers to the screen in decimal
extern void putd(u64 i);

#endif