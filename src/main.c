/*
    A main entry of the TAIL bootloader.

    This is a second stage which provides the I/O interactions with user
    and load the kernel itself.
*/

// Includes
#include<inttypes.h>

#include "memory_map.h"
#include "vga/vga.h"

// Screen definitions.
#define LOADER   0b00
#define SETTINGS 0b01
#define TERMINAL 0b10
#define INFO     0b11

// Draws the main loader screen.
// void draw_loader();

/* A global variable that provide the settings information.

The lower three bits decide about the text color, the next three
bits are the background color and the next two bits are the current user's screen.
This can be changed via settings screen. */
static volatile uint16_t CURRENT_SETTINGS = LOADER << 7 | COLOR_BLACK << 4 | COLOR_WHITE;

// The main entry. The boot drive argument is pushed from the HEAD part (mbr boot sector).
void main(uint16_t boot_drive) {
    VGABuffer buf = {.row = 0, .col = 0};
    enable_cursor(0, 15);
    
    println(
        "Hello cursor.",
        CURRENT_SETTINGS & COLOR_MASK,
        (CURRENT_SETTINGS >> 4) & COLOR_MASK,
        &buf
    );

    update_cursor(8, 77);
    
    while(1) {}
}


/* Draws a loader screen. */
// void draw_loader() {

// }