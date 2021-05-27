#include <int_types.h>
#include <exceptions.h>
#include <paging.h>
#include <print.h>

__attribute__((interrupt)) void halting_interrupt_handler(InterruptFrame* frame){
    puts("UNHANDLED INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void div_by_zero_interrupt(InterruptFrame* frame){
    puts("DIV BY ZERO INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void debug_interrupt(InterruptFrame* frame){
    puts("DEBUG INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void nmi_interrupt(InterruptFrame* frame){
    puts("NON MASKABLE INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void breakpoint(InterruptFrame* frame){
    puts("BREAKPOINT INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void overflow_interrupt(InterruptFrame* frame){
    puts("OVERFLOW INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void bound_range_exceeded_interrupt(InterruptFrame* frame){
    puts("BOUND RANGE EXCEEDED INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void invalid_opcode_interrupt(InterruptFrame* frame){
    puts("INVALID OPCODE INTERRUPT");
    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void device_not_available_interrupt(InterruptFrame* frame){
    puts("DEVICE NOT AVAILABLE INTERRUPT");
    __asm__ __volatile("hlt");
}

__attribute__((interrupt)) void double_fault_interrupt(InterruptFrame* frame, u64 err){
    puts("DOUBLE FAULT INTERRUPT");
    __asm__ __volatile("hlt");
}

__attribute__((interrupt)) void coprocessor_segment_overrun_interrupt(InterruptFrame* frame){
    puts("COPROCESSOR SEGMENT OVERRUN INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void invalid_tss_interrupt(InterruptFrame* frame, u64 err){
    puts("INVALID TSS INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void segment_not_present_interrupt(InterruptFrame* frame, u64 err){
    puts("SEGMENT NOT PRESENT INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void stack_segment_fault_interrupt(InterruptFrame* frame, u64 err){
    puts("STACK SEGMENT FAULT INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void general_protection_fault_interrupt(InterruptFrame* frame, u64 err){
    puts("GENERAL PROTECTION FAULT INTERRUPT\n");
    puts("error code = ");
    putx64(err);
    putc('\n');
    puts("rip = ");
    putx64(frame->rip);
    putc('\n');


    u64 cr3;
    __asm__ __volatile__(
        "mov %%cr3, %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=m" (cr3)
    );

    print_pagetable((PageTableEntry*)cr3, 4);

    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void page_fault_handler(InterruptFrame* frame, u64 errcode){
    puts("PAGE FAULT EXCEPTION\n");
    puts("error code = ");
    putx8(errcode);
    putc('\n');


    // get the value of cr2 register - this is the address that is being accessed
    u64 cr2, cr3;
    __asm__ __volatile__(
        "mov %%cr2, %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=m" (cr2)
    );

    __asm__ __volatile__(
        "mov %%cr3, %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=m" (cr3)
    );


    puts("CR2 = ");
    putx64(cr2);

    print_pagetable((PageTableEntry*)cr3, 4);

    __asm__ __volatile__("hlt");
}

__attribute__((interrupt)) void x87_fpu_interrupt(InterruptFrame* frame){
    puts("X87 FLOATING POINT EXCEPTION INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void alignment_check_interrupt(InterruptFrame* frame, u64 err){
    puts("ALIGNMENT CHECK INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void machine_check_interrupt(InterruptFrame* frame){
    puts("MACHINE CHECK INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void simd_floating_point_interrupt(InterruptFrame* frame){
    puts("SIMD FLOATING POINT EXCEPTION");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void virtualization_interrupt(InterruptFrame* frame){
    puts("VIRTUALIZATION INTERRUPT");
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void security_exception_interrupt(InterruptFrame* frame, u64 err){
    puts("SECURITY EXCEPTION");
    __asm__ __volatile__("hlt");
}
