/*
    A main entry of the TAIL bootloader.

    This is a second stage which provides the I/O interactions with user
    and load the kernel itself.
*/

extern void *IDT_TABLE[];       // IDT TABLE defined in idt.asm 

#define __PIC_MASTER_OFFSET__ 32

#include <stdint.h>
#include "arch/idt.h"
#include "arch/pic.h"
#include "insc.h"
#include "vga.h"

void init_menu(void);
void draw_menu(void);

// Contain all provided addresses of installed operating systems.
#ifdef ARRAY_ADDRS
static uint64_t ADDRS[] = { ARRAY_ADDRS };
#endif

// A global static buffer.
volatile VGABuffer LOGGER = {.row = 0, .col = 0};

// Jump pointer to OS selected by user.
uint64_t jptr;

/* Main daemon entry point. */
void main(uint16_t boot_drive) {
    disable_cursor(); 

#if !__RELEASE__
    prints("Initializing... ", L_INFO, &LOGGER); 
#endif

    /* Beginning of daemon post initialization */
    
    idt_init();                             // Intializing the IDT.
    remap_pic(__PIC_MASTER_OFFSET__);       // Remapping PIC controller.

    /* End of initialization */
    
#if !__RELEASE__
    println(OK, L_OK, &LOGGER);
#endif

    init_menu();    // Holds user in menu until the OS is not selected or timed-out

#ifdef ADDRS
    // Forwarding control to the OS.
    __asm__ ("jmp %0" : "=r" (jptr)); 
#endif

    for(;;) __asm__ ("hlt");           // Halts the bootloader if no addres was selected. 
}

/* Initializing the IDT and putting required handler functions within */
void idt_init() {
    uint16_t cs;
    uint8_t vec;

    // Reading the cs segment.
    __asm__("mov %%cs, %w0\n":"=a"(cs)::"memory");

    // Providing exceptions.
    for (vec = 0; vec < 255; vec++) {
        idt_set_descriptor(vec, (ISR_F)IDT_TABLE[vec], TRAP_GATE, cs);
    }
    // Providing interrupts.
    for (; vec < 255; vec++) {
        idt_set_descriptor(vec, (ISR_F)IDT_TABLE[vec], INT_GATE, cs);
    }

    // Creating a pointer for lidt instruction
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { .length = sizeof(IDT) - 1, .base =  &IDT };
 
    __asm__ ( "lidt %0" : : "m"(IDTR) );
}

// Initializes the main TUI menu for choosing the OS. 
void init_menu() {
    draw_menu();

    __asm__ ("sti");
    while (os_id == UNSELECTED);    // Interrupt driven, until the OS is selected or timeout.
}

void line(uint8_t amount) {
    for (uint8_t i = 0; i < amount; ++i)
        printc('\xcd', COLOR_WHITE, &LOGGER);
}

void walls() {
    printc('\xba', COLOR_WHITE, &LOGGER);
    LOGGER.col = BUFFER_WIDTH - 1;
    printc('\xba', COLOR_WHITE, &LOGGER);
}

// Prints a short hint in the bottom box.
void print_hint(char* info) {
    LOGGER.row = BUFFER_HEIGHT - 3;
    LOGGER.col = 3;
    prints(info, COLOR_YELLOW, &LOGGER);
}

/* Draws the select menu itself */
void draw_menu() {
    LOGGER = (VGABuffer) {.row = 0, .col = 0};
    vga_clean();

    printc('\xc9', COLOR_WHITE, &LOGGER);
    line(BUFFER_WIDTH - 7);
    prints("\xcb\xcd\xcd\xcd\xcd\xbb", COLOR_WHITE, &LOGGER);

    prints("\xba TAIL V.0.1", COLOR_WHITE, &LOGGER);
    LOGGER.col = BUFFER_WIDTH - 6;
    prints("\xba XX \xba", COLOR_WHITE, &LOGGER);

    printc('\xcc', COLOR_WHITE, &LOGGER);
    line(BUFFER_WIDTH - 7);
    prints("\xca\xcd\xcd\xcd\xcd\xb9", COLOR_WHITE, &LOGGER);

    prints("\xba  Select the OS to load:", COLOR_WHITE, &LOGGER);
    LOGGER.col = BUFFER_WIDTH - 1;
    printc('\xba', COLOR_WHITE, &LOGGER);

    for (uint8_t i = 0; i < 16; ++i) {
        printc('\xba', COLOR_WHITE, &LOGGER);
        prints("  --------------------------------------------------------------------------  ", COLOR_DARKGRAY, &LOGGER);
        printc('\xba', COLOR_WHITE, &LOGGER);
    }

    walls();

    printc('\xcc', COLOR_WHITE, &LOGGER);
    line(BUFFER_WIDTH - 2);
    printc('\xb9', COLOR_WHITE, &LOGGER);

    walls();
    walls();

    printc('\xc8', COLOR_WHITE, &LOGGER);
    line(BUFFER_WIDTH - 2);
    printc('\xbc', COLOR_WHITE, &LOGGER);

    print_hint("Use arrow keys to change the cursor. Press 'i' for more info or 's' to open\n" 
               "   a settings menu. Press 't' to open a terminal.");
}
