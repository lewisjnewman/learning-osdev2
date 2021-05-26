#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <int_types.h>

typedef struct{
    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
} InterruptFrame;


extern __attribute__((interrupt)) void halting_interrupt_handler(InterruptFrame* frame);
extern __attribute__((interrupt)) void page_fault_handler(InterruptFrame* frame, u64 errcode);

#endif