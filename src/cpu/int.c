/* 
 * A module for setting up the interrupts.
 *
 * This module sets up the IDT for handling the bootloader itself, not the future OS and 
 * defines some basic interface to comunicate with the hardware by wrapping the asm instructions.
 * Since the application is rather small, there is no need of using most of the handlers,
 * hovewer some important ones must always exist, especially some specific exceptions. Regular
 * software interupts must also set, because in protected mode we cannot use BIOS anymore, therefore
 * PIC controller that sets some interrupts must be used for I/O.
 * 
 * */

#include<stdint.h>

#include"int.h"
#include"pic.h"
#include"handlers.h"

#include"../io/vga.h"

// Externs
extern uint8_t CURRENT_SETTINGS;
extern VGABuffer BUFF;

__attribute__((no_caller_saved_registers)) 
extern void end_of_interrupt(uint16_t port);

// Constant conversions for code simplicity in the main.
const HANDLER_FN DIVBZ = { .f = divbz };
const HANDLER_FN PIC_TIMER = {.f = pic_timer };

/*------- Definitions of the handler functions. ------*/

__attribute__((interrupt)) void divbz(struct Iframe *frame) {
    shutdown(); // For now shutting down the machine.
}

__attribute__((interrupt)) void pic_timer(struct Iframe *frame) {
    prints(".", CURRENT_SETTINGS, &BUFF); // Printing something.
    
    // Calling an end of interrupt.
    end_of_interrupt(PIC1_COMMAND);
}
