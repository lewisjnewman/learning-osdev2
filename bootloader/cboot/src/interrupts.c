#include <interrupts.h>
#include <exceptions.h>
#include <int_types.h>

typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) IDTR;

typedef struct {
    u16 ist_index:3;
    u16 _reserved1:5;
    u16 gatetype:1;
    u16 _reserved2:3;
    u16 _reserved3:1;
    u16 privilege:2;
    u16 present:1;
} __attribute__((packed)) IDTOptions;

typedef struct {
    u16 offset_low;
    u16 selector;
    IDTOptions options;
    u16 offset_mid;
    u32 offset_high;
    u32 zero2;
}  __attribute__((packed)) IDT_Descriptor;

IDT_Descriptor IDT[32];

IDTOptions new_options(){
    IDTOptions options;
    options.ist_index = 0;
    options.gatetype = 0;
    options._reserved1 = 0;
    options._reserved2 = ~0;
    options._reserved3 = 0;
    options.privilege = 0;
    options.present = 1;
    return options;
}

void set_interrupt(usize num, u64 addr){

    IDT[num].selector = 0x8;
    IDT[num].offset_low = (u16)addr;
    IDT[num].offset_mid = (u16)(addr>>16);
    IDT[num].offset_high = (u32)(addr>>32);
    IDT[num].options = new_options();
    IDT[num].zero2 = 0;

}

void lidt() {
    IDTR idtr;
    idtr.base = (u64)IDT;
    idtr.limit = sizeof(IDT)-1;

    __asm__ ("lidt %0" :: "m"(idtr));
}

void setup_interrupts(){

    set_interrupt(0, (u64)div_by_zero_interrupt);
    set_interrupt(1, (u64)debug_interrupt);
    set_interrupt(2, (u64)nmi_interrupt);
    set_interrupt(3, (u64)breakpoint);
    set_interrupt(4, (u64)overflow_interrupt);
    set_interrupt(5, (u64)bound_range_exceeded_interrupt);
    set_interrupt(6, (u64)invalid_opcode_interrupt);
    set_interrupt(7, (u64)device_not_available_interrupt);
    set_interrupt(8, (u64)double_fault_interrupt);
    set_interrupt(9, (u64)coprocessor_segment_overrun_interrupt);
    set_interrupt(10, (u64)invalid_tss_interrupt);
    set_interrupt(11, (u64)segment_not_present_interrupt);
    set_interrupt(12, (u64)stack_segment_fault_interrupt);
    set_interrupt(13, (u64)general_protection_fault_interrupt);
    set_interrupt(14, (u64)page_fault_handler);
    set_interrupt(15, (u64)halting_interrupt_handler);
    set_interrupt(16, (u64)x87_fpu_interrupt);
    set_interrupt(17, (u64)alignment_check_interrupt);
    set_interrupt(18, (u64)machine_check_interrupt);
    set_interrupt(19, (u64)simd_floating_point_interrupt);
    set_interrupt(20, (u64)virtualization_interrupt);
    set_interrupt(21, (u64)halting_interrupt_handler);
    set_interrupt(22, (u64)halting_interrupt_handler);
    set_interrupt(23, (u64)halting_interrupt_handler);
    set_interrupt(24, (u64)halting_interrupt_handler);
    set_interrupt(25, (u64)halting_interrupt_handler);
    set_interrupt(26, (u64)halting_interrupt_handler);
    set_interrupt(27, (u64)halting_interrupt_handler);
    set_interrupt(28, (u64)halting_interrupt_handler);
    set_interrupt(29, (u64)halting_interrupt_handler);
    set_interrupt(30, (u64)security_exception_interrupt);
    set_interrupt(31, (u64)halting_interrupt_handler);

    lidt();
}