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

// just a generic interrupt handler
extern __attribute__((interrupt)) void halting_interrupt_handler(InterruptFrame* frame);

extern __attribute__((interrupt)) void halting_interrupt_handler(InterruptFrame* frame);
extern __attribute__((interrupt)) void div_by_zero_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void debug_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void nmi_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void breakpoint(InterruptFrame* frame);
extern __attribute__((interrupt)) void overflow_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void bound_range_exceeded_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void invalid_opcode_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void device_not_available_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void double_fault_interrupt(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void coprocessor_segment_overrun_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void invalid_tss_interrupt(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void segment_not_present_interrupt(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void stack_segment_fault_interrupt(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void general_protection_fault_interrupt(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void page_fault_handler(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void x87_fpu_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void alignment_check_interrupt(InterruptFrame* frame, u64 err);
extern __attribute__((interrupt)) void machine_check_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void simd_floating_point_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void virtualization_interrupt(InterruptFrame* frame);
extern __attribute__((interrupt)) void security_exception_interrupt(InterruptFrame* frame, u64 err);

#endif