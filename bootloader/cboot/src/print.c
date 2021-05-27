#include <print.h>
#include <vga.h>
#include <mem.h>
#include <serial.h>

void putc(char c){
    draw_char(c);
    serial_write_char(c);
}

void puts(char* s){
    while(*s != '\0') {
        putc(*s);
        s++;
    }
}

char hexdigits[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

void putx8(u8 val){
    char buf[20];
    memset(buf, 0, 20);
    for(usize i = 0; i < sizeof(val)*2; i++){
        buf[sizeof(val)*2-1-i] = hexdigits[(val>>(i*4))&0xF];
    }
    puts(buf);
}

void putx16(u16 val){
    char buf[20];
    memset(buf, 0, 20);
    for(usize i = 0; i < sizeof(val)*2; i++){
        buf[sizeof(val)*2-1-i] = hexdigits[(val>>(i*4))&0xF];
    }
    puts(buf);
}

void putx32(u32 val){
    char buf[20];
    memset(buf, 0, 20);
    for(usize i = 0; i < sizeof(val)*2; i++){
        buf[sizeof(val)*2-1-i] = hexdigits[(val>>(i*4))&0xF];
    }
    puts(buf);
}

void putx64(u64 val){
    char buf[20];
    memset(buf, 0, 20);
    for(usize i = 0; i < sizeof(val)*2; i++){
        buf[sizeof(val)*2-1-i] = hexdigits[(val>>(i*4))&0xF];
    }
    puts(buf);
}


void putd(u64 val){
    char buf[32];
    memset(buf, 0, 32);
    char* s = &buf[31];

    *--s = 0;
    if (!val) *--s = '0';
    for(; val; val/=10) *--s = '0' + val%10;
    puts(s);
}