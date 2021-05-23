#include <int_types.h>

#include <int_types.h>

// x86-64 io port interactions
u8 io_in8 (u16 port);
u16 io_in16 (u16 port);
u32 io_int32(u16 port);

void io_out8 (u16 port, u8 data);
void io_out16 (u16 port, u16 data);
void io_out32(u16 port, u32 value);