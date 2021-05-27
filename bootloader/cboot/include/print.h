#ifndef PRINT_H
#define PRINT_H

#include <int_types.h>

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