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
    for(usize i = 0; i < 32; i++){
        set_interrupt(i, (u64)halting_interrupt_handler);
    }

    set_interrupt(14, (u64)page_fault_handler);


    lidt();
}