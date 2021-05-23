
#include <int_types.h>

typedef struct{
    u64 ip;         // instruction pointer
    u64 cs;         // code segment
    u64 flags;      // flags register
    u64 sp;         // stack pointer
    u64 ss;         // stack segment
} interrupt_frame;


extern void setup_interrupts();