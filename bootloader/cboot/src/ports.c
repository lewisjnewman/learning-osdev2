#include <int_types.h>

// x86-64 io port interactions

// read an 8bit value from an io port
u8 io_in8 (u16 port) {
    u8 result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

// read a 16bit value from an io port
u16 io_in16 (u16 port) {
    u16 result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

// read a 32bit value from an io port
u32 io_int32(u16 port) {
    u32 result;
    __asm__ __volatile__("inl %%dx,%%eax":"=a" (result):"d"(port));
    return result;
}

// write an 8bit value to an io port
void io_out8 (u16 port, u8 data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

// write a 16bit value to an io port
void io_out16 (u16 port, u16 data) {
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

// write a 32bit value to an io port
void io_out32(u16 port, u32 value) {
    __asm__ __volatile__("outl %%eax,%%dx"::"d" (port), "a" (value));
};
