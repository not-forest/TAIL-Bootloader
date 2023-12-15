/*
    A main entry of the TAIL bootloader.

    This is a second stage which provides the I/O interactions with user
    and load the kernel itself.
*/

// Includes
#include<inttypes.h>

#include "memory_map.h"

#include "vga/vga.h"
#include "vga/presets.h"

// Screen definitions.
#define LOADER   0b00
#define SETTINGS 0b01
#define TERMINAL 0b10
#define INFO     0b11

// Draws the info screen with the logo. 
void draw_info(uint_fast8_t row);
// Draws the main loader screen.
void draw_loader();

/* A global variable that provide the settings information.

The lower three bits decide about the text color, the next three
bits are the background color and the next two bits are the current user's screen.
This can be changed via settings screen. */
static volatile uint16_t CURRENT_SETTINGS = LOADER << 7 | COLOR_BLACK << 4 | COLOR_WHITE;

void main(uint16_t boot_drive) {
    draw_info(1);

    while(1) {}
}


/* For each row within the info screen the corresponding symbols will be written. */
void draw_info(uint_fast8_t row) {
    
    switch (row)
    {
    case 0:
    case 6:
    case 7:
    case 13:
        break;
    default:
        VGABuffer i_vga = {.row = row, .col = 0};
        
        prints(INFO_SCREEN_DATA[row], CURRENT_SETTINGS & COLOR_MASK, SETTINGS >> 4 & COLOR_MASK, &i_vga);
    }
}