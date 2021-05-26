#include <int_types.h>
#include <exceptions.h>
#include <vga.h>

__attribute__((interrupt)) void halting_interrupt_handler(InterruptFrame* frame){
    puts("UNHANDLED INTERRUPT");
    __asm__ __volatile__("hlt");
}

char* page_fault_err_messages[] = {
    "Supervisory process tried to read a non-present page entry\n",
    "Supervisory process tried to read a page and caused a protection fault\n",
    "Supervisory process tried to write to a non-present page entry\n",
    "Supervisory process tried to write a page and caused a protection fault\n",
    "User process tried to read a non-present page entry\n",
    "User process tried to read a page and caused a protection fault\n",
    "User process tried to write to a non-present page entry\n",
    "User process tried to write a page and caused a protection fault\n"
};

__attribute__((interrupt)) void page_fault_handler(InterruptFrame* frame, u64 errcode){
    puts("PAGE FAULT EXCEPTION\n");
    puts("error code = ");
    putx8(errcode);
    putc('\n');


    // get the value of cr2 register - this is the address that is being accessed
    u64 cr2;
    __asm__ __volatile__(
        "mov %%cr2, %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=m" (cr2)
    );

    puts("CR2 = ");
    putx64(cr2);

    __asm__ __volatile__("hlt");
}
