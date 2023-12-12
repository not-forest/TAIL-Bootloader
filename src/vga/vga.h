/*
    This module provides operations with VGA Buffer. The bootloader should not
    define some crazy abstractions of the way how it should comunicate with ports,
    therefore the small implementation is enough to provide information to the user.
*/

#pragma once

#ifndef _VGA_INTERFACE_
#define _VGA_INTERFACE_

#include<inttypes.h>

#define BUFFER_PTR 0xb8000
#define BUFFER_WIDTH 80
#define BUFFER_HEIGHT 20

#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x1
#define COLOR_GREEN 0x2
#define COLOR_CYAN 0x3
#define COLOR_RED 0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN 0x6
#define COLOR_LIGHTGRAY 0x7
#define COLOR_DARKGRAY 0x8
#define COLOR_LIGHTBLUE 0x9
#define COLOR_LIGHTGREEN 0xa
#define COLOR_LIGHTCYAN 0xb
#define COLOR_LIGHTRED 0xc
#define COLOR_LIGHTMAGENTA 0xd
#define COLOR_YELLOW 0xe
#define COLOR_WHITE 0xf

// A struct that tracks the cursor to print out messages correctly.
typedef struct {
    uint8_t row, col;
} VGABuffer;

/* Prints out a single char by writing the data into the buffer.

the background and foreground colors will be provided with the char. If the
fourth bit of the background color is set, the character will blink like the
underscore symbol. If the fourth bit of the foreground color is set, the
character will be bright. */
void printc(unsigned char c, uint8_t fr, uint8_t bg, VGABuffer* vga) {
    // Pointer to the VGA buffer.
    volatile uint16_t* buf = (volatile uint16_t*)BUFFER_PTR + vga->col;
    uint16_t char_set = (bg << 12) | (fr << 8) | c; // Creating a formatted char for buffer.

    *buf = char_set; // Writing to the buffer and changing the cursor's position.
    ++vga->col;
}

/* Prints the provided string into the buffer.

The provided atributes are used to every symbol within the string. */
void prints(char* str, uint8_t fr, uint8_t bg, VGABuffer* vga) {
    while(*str) {
        printc(*str, fr, bg, vga);
        ++str;
    }
}

#endif