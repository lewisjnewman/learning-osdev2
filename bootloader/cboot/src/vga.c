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

void draw_char(char c){
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