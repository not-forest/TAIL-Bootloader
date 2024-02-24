/* 
 * This module defines all handlers functions required for the bootloader to operate
 * and provide the minimal I/O communication with the user.
 * */
#pragma once

#include<stdint.h>

struct Iframe;

/* 
 * An exception handler function
 *
 * Handles the division by zero exception in the bootloader (INT = 0x0), which can occur
 * due to some unecessary commands sended to the terminal by user.
 * */
__attribute__((interrupt)) void divbz(struct Iframe *frame);

/* 
 * A software interrupt handler.
 *
 * Handles the ticking timer's interrupt from the master PIC chip.
 * */
__attribute__((interrupt)) void pic_timer(struct Iframe *frame);

/* 
 * A handy union that not only represents the handler function itself, but also allows for quick and
 * efficient writing it's pointer as a digit value, that can be used in the gate descriptor.
 * */
typedef union {
    void *f;
    uint16_t bits[2];
} HANDLER_FN;

/* 
 * A special struct that is being pushed to the stack by CPU when some exception or interrupt occurs.
 *
 * In our protected mode the cpu will push three values onto the stack, which allows to take actions
 * based on the recieved values.
 * */
struct Iframe {
    uint16_t eip, cs, eflags;
};
