/* 
 * This module provides operations with VGA Buffer. The bootloader should not 
 * define some crazy abstractions of the way how it should comunicate with ports, 
 * therefore the small implementation is enough to provide information to the user.
 * */

#include<stdint.h>
#include"vga.h"

/* Shifts the values in the buffer by rows 
The function is not suitable for switching rows, but only shift to a
given location. Values must be within the height of the buffer. */
void vga_shift(uint_fast8_t old, uint_fast8_t new) {
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
void vga_swap(uint_fast8_t old, uint_fast8_t new) {
    // Pointer to old row buffer.
    volatile uint16_t* old_ptr = (volatile uint16_t*)BUFFER_PTR + old * BUFFER_WIDTH;
    // Pointer to new row buffer.
    volatile uint16_t* new_ptr = old_ptr - (old - new) * BUFFER_WIDTH;

    /* Shifts all values in the buffer by each double word.
    The x86 xchg instruction is being used. */
    for(uint8_t col_offset = 0; col_offset < BUFFER_WIDTH; col_offset += 1) {
        __asm__ (
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
__attribute__((no_caller_saved_registers))
int printc(const unsigned char c, uint8_t color_set, volatile VGABuffer* vga) {
    /* Special case for the null character */
    if (c == '\0') {
        return 0;
    }
    
    /* Special case for new line and space characters */
    if (c != '\n' && c != ' ') {
        // Pointer to the VGA buffer.
        volatile uint16_t* buf = (volatile uint16_t*)BUFFER_PTR + (vga->row * BUFFER_WIDTH + vga->col);
        uint16_t char_set = (color_set << 8) | c; // Creating a formatted char for buffer.

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
__attribute__((no_caller_saved_registers))
void prints(const char* str, uint8_t color_set, volatile VGABuffer* vga) {
    while(printc(*str, color_set, vga)) {
        ++str;
    }
}

/* Works the same as the regular 'prints' except that is adds a newline symbol. */
__attribute__((no_caller_saved_registers))
void println(const char* str, uint8_t color_set, volatile VGABuffer* vga) {
    prints(str, color_set, vga);
    vga->col = 0;
    ++vga->row;
}

// Disables the cursor in the VGA mode.
void disable_cursor() {
	__asm__ volatile (
        "mov $0x3d4, %%dx\n"
        "mov $0xa, %%al\n"
        "out %%al, %%dx\n"
        
        "inc %%dx\n"
        "mov $0x20, %%al\n"
        "out %%al, %%dx"

        :
        :
        : "dx", "al", "memory"
    );
}

/* Enables the cursor, with given start and end

The start and end values are basically describe the height by rows of the cursor. The
start must be smaller than end for cursor to be visible. The maximum value of both
start and end is 15.*/
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    __asm__ volatile (
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
        : "dx", "al", "memory"
    );
}

// Updates the cursor location based on a row and column value.
void update_cursor(uint8_t row, uint8_t col) {
    uint16_t pos = row * BUFFER_WIDTH + col;
    __asm__ volatile (
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
        : "memory"
    );
}
