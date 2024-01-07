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
#define BUFFER_HEIGHT 25

#define COLOR_MASK 0xf

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
    uint_fast8_t row, col;
} VGABuffer;

/* Shifts the values in the buffer by rows 

The function is not suitable for switching rows, but only shift to a
given location. Values must be within the height of the buffer. */
static inline void vga_shift(uint_fast8_t old, uint_fast8_t new) {
    // Pointer to old row buffer.
    volatile uint16_t* old_ptr = (volatile uint16_t*)BUFFER_PTR + old * BUFFER_WIDTH;
    // Pointer to new row buffer.
    volatile uint16_t* new_ptr = old_ptr - (old - new) * BUFFER_WIDTH;

    // Shifts all values in the buffer by each double word.
    for(uint8_t col_offset = 0; col_offset < BUFFER_WIDTH; col_offset += 1) {
        *(new_ptr + col_offset) = *(old_ptr + col_offset);
    }
}

/* Swaps two rows instead of shifting one to another. 

The swapping is done via XOR operation for each bit*/
static inline void vga_swap(uint_fast8_t old, uint_fast8_t new) {
    // Pointer to old row buffer.
    volatile uint16_t* old_ptr = (volatile uint16_t*)BUFFER_PTR + old * BUFFER_WIDTH;
    // Pointer to new row buffer.
    volatile uint16_t* new_ptr = old_ptr - (old - new) * BUFFER_WIDTH;

    /* Shifts all values in the buffer by each double word.
    The x86 xchg instruction is being used. */
    for(uint8_t col_offset = 0; col_offset < BUFFER_WIDTH; col_offset += 1) {
        asm (
            "xchg %0, %1"
            : "=r"(*(new_ptr + col_offset))
            : "m"(*(old_ptr + col_offset)), ""(*(new_ptr + col_offset))
            : "memory"
        );
    }
}

/* Prints out a single char by writing the data into the buffer.

the background and foreground colors will be provided with the char. If the
fourth bit of the background color is set, the character will blink like the
underscore symbol. If the fourth bit of the foreground color is set, the
character will be bright.

It will always return 1 as long as it will not receive a null character.*/
int printc(unsigned char c, uint8_t fr, uint8_t bg, VGABuffer* vga) {
    /* Special case for the null character */
    if (c == '\0') {
        return 0;
    }
    
    /* Special case for new line and space characters */
    if (c != '\n' && c != ' ') {
        // Pointer to the VGA buffer.
        volatile uint16_t* buf = (volatile uint16_t*)BUFFER_PTR + (vga->row * BUFFER_WIDTH + vga->col);
        uint16_t char_set = (bg << 12) | (fr << 8) | c; // Creating a formatted char for buffer.

        *buf = char_set; // Writing to the buffer and changing the cursor's position.
    }

    /* Checking the buffer bounds */
    if (c == '\n' || ++vga->col >= BUFFER_WIDTH) {
        vga->col = 0;
        if (vga->row + 1 >= BUFFER_HEIGHT) {
            /* Shifting each row up. */
            for(uint8_t r = 1; r < BUFFER_HEIGHT; ++r) {
                vga_shift(r, r - 1);
            }
        } else {
            ++vga->row;
        }
    }

    return 1;
}

/* Prints the provided string into the buffer.

The provided atributes are used to every symbol within the string. */
void prints(char* str, uint8_t fr, uint8_t bg, VGABuffer* vga) {
    while(printc(*str, fr, bg, vga)) {
        ++str;
    }
}

/* Works the same as the regular 'prints' except that is adds a newline symbol. */
static inline void println(char* str, uint8_t fr, uint8_t bg, VGABuffer* vga) {
    prints(str, fr, bg, vga);
    vga->col = 0;
    ++vga->row;
}

// Disables the cursor in the VGA mode.
static inline void disable_cursor() {
	asm volatile (
        "mov $0x3d4, %%dx\n"
        "mov $0xa, %%al\n"
        "out %%al, %%dx\n"
        
        "inc %%dx\n"
        "mov $0x20, %%al\n"
        "out %%al, %%dx"

        :
        :
        : "dx", "al"
    );
}

/* Enables the cursor, with given start and end

The start and end values are basically describe the height by rows of the cursor. The
start must be smaller than end for cursor to be visible. The maximum value of both
start and end is 15.*/
static inline void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    asm volatile (
        // Setting up the cursor's start
        "mov $0x3d4, %%dx\n"
        "mov $0xa, %%al\n"
        "out %%al, %%dx\n"

        "inc %%dx\n"
        "inb %%dx, %%al\n"
        "and $0xc0, %%al\n"
        "or %0, %%al\n"
        "out %%al, %%dx\n"

        // Setting up the cursor's start
        "mov $0x3d4, %%dx\n"
        "mov $0xb, %%al\n"
        "out %%al, %%dx\n"

        "inc %%dx\n"
        "inb %%dx, %%al\n"
        "and $0xe0, %%al\n"
        "or %1, %%al\n"
        "out %%al, %%dx\n"

        :
        : "r" (cursor_start), "r" (cursor_end)
        : "dx", "al"
    );
}

void update_cursor(uint8_t row, uint8_t col) {
    uint16_t pos = row * BUFFER_WIDTH + col;
    asm volatile (
        // Moving the cursor
        "mov $0x03d4, %%dx\n"
        "mov $0x0f, %%al\n"
        "out %%al, %%dx\n"

        "inc %%dl\n"
        "mov %%bl, %%al\n"
        "out %%al, %%dx\n"

        "dec %%dl\n"
        "mov $0x0e, %%al\n"
        "out %%al, %%dx\n"

        "inc %%dl\n"
        "mov %%bh, %%al\n"
        "out %%al, %%dx\n"
        :
        : "b"(pos)
        :
    );
}


#endif