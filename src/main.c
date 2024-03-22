/*
    A main entry of the TAIL bootloader.

    This is a second stage which provides the I/O interactions with user
    and load the kernel itself.
*/

// Includes
#include<stdint.h>

#include"memory_map.h"

#include"io/vga.h"

#include"cpu/pic.h"
#include"cpu/int.h"
#include"cpu/handlers.h"

// Screen definitions.
#define LOADER   0b00
#define SETTINGS 0b01
#define TERMINAL 0b10
#define INFO     0b11

void draw_loader();
void init();

// A global static buffer.
volatile VGABuffer BUFF = {.row = 0, .col = 0};

/* A global variable that provide the settings information.
 *
 * The lower four bits decide about the text color, the next four bits are the background
 * color and the last two bits are the current user's screen. This can be changed via 
 * settings screen. 
 *
 * */
volatile union {
    struct {
        uint8_t fr: 4;
        uint8_t bg: 4;
        uint8_t screen: 2; 
    } fields;
    uint8_t color_set;
    uint16_t data;
} CURRENT_SETTINGS = { .fields.fr = COLOR_WHITE, .fields.bg = COLOR_BLACK, .fields.screen = LOADER };

// The main entry. The boot drive argument is pushed from the HEAD part (mbr boot sector).
void main(uint16_t boot_drive) {
    // Initialize
    init();
    // TUI
    draw_loader();  // Drawing the loader screen first.

    for(;;) wfi();  // Waits for interrupt
}

// Initialize the bootloader itself. This does not affect the future OS.
void init() {
    // Defining the new IDT.
    IDT idt;
    uint16_t cs;

    // Reading the cs segment.
    __asm__("mov %%cs, %w0\n":"=a"(cs)::"memory");

    // Defining the interrupt and exception handlers. 
    idt.vector_table[0] = (GateDescriptor) { 
        .offset_0_15 = DIVBZ.bits[0],
        .selector = cs,
        .reserved = 0,
        .attr = TRAP_GATE, 
        .offset_16_31 = DIVBZ.bits[1],
    }; 
    
    idt.vector_table[32] = (GateDescriptor) { 
        .offset_0_15 = PIC_TIMER.bits[0],
        .selector = cs,
        .reserved = 0,
        .attr = INT_GATE, 
        .offset_16_31 = PIC_TIMER.bits[1],
    };

    // Final initialization functions.
    lidt(&idt); // Setting the amount of interrupts needed for the bootloader.
    remap_pic(32); // Mapping the PIC software interrupts to 32th index of the IDT.
}

// Draws the main menu of the loader.
void draw_loader() {

}

