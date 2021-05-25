#include <int_types.h>
#include <vga.h>
#include <mem.h>

typedef struct{
    char character;
    u8 colour;
} __attribute__((packed)) VGAChar;

#define VGA_ROWS 25
#define VGA_COLS 80

VGAChar* vga_buffer = (VGAChar*)0xb8000;
usize column_counter = 0;
BiosColours foreground = LIGHT_GRAY;
BiosColours background = BLACK<<4;

void newline(){
    // shift every row up by 1 except for the very first row which is overwritten
    column_counter = 0;
    for(usize i = 1; i < VGA_ROWS; i++){
        for(usize j = 0; j < VGA_COLS; j++){
            vga_buffer[VGA_COLS * (i-1) + j] = vga_buffer[VGA_COLS * i + j];
        }
    }

    // Clear the final row
    VGAChar blank;
    blank.character = ' ';
    blank.colour = foreground | background;
    for(usize j = 0; j < VGA_COLS; j++){
        vga_buffer[VGA_COLS * 24 + j] = blank;
    }

}

void clear_screen(){
    VGAChar blank;
    blank.character = ' ';
    blank.colour = foreground | background;
    for(usize i = 0; i < VGA_ROWS; i++){
        for(usize j = 0; j < VGA_COLS; j++){
            vga_buffer[VGA_COLS * i + j] = blank;
        }
    }
}

void set_foreground(BiosColours colour){
    foreground = (colour) & 0xF0;
}

void set_background(BiosColours colour){
    background = (colour<<4) & 0x0F;
}

void putc(char c){
    if (column_counter >= VGA_COLS) {
        newline();
    } else if (c == '\n'){
        newline();
    } else if (c == '\r'){
        column_counter = 0;
    } else {
        VGAChar vc;
        vc.character = c;
        vc.colour = foreground | background;
        vga_buffer[VGA_COLS * 24 + column_counter] = vc;
        column_counter++;
    }
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