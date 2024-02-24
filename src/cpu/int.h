/* 
 * Module for interrupting the cpu and call some close to hardware code.
 *
 * All functions that wraps an assembly alternatives is defined within this
 * module.
 * */
#pragma once

#include <stdint.h>
#include "segm.h"

#define TASK_GATE   0x85
#define INT_GATE    0x8E
#define TRAP_GATE   0x8f

// A special type for gate descriptor, which is an entry in the IDT.
typedef struct {
    uint16_t offset_0_15;
    uint16_t selector;  // Code segment selector in GDT.
    uint8_t reserved;   // Must always be set to zero
    uint8_t attr;       // Contain gate type, dpl and p fields. 
    uint16_t offset_16_31;
} __attribute__((packed)) GateDescriptor;

/* A type representing the vector table itself.
 *
 * It contains 256 entries of interrupt vectors which must point to the handler function, that
 * will handle the CPU exception or software interrupt. Since only some fields really makes sense
 * in terms of bootloader, not all fields will be filled, because it is an OS's job to cover all vectors.
 * */
typedef struct {
    GateDescriptor vector_table[256];
} IDT;

/* 
 * Waits for interrupts 
 *
 * Just a wrapper that waits for the interrupts to occur, so it enables them and hlts the processor. The user's keyboard 
 * */
__attribute__((no_caller_saved_registers)) 
static inline void wfi() {
    __asm__ (
        "sti\n"
        "hlt"
    );
}

// Writes a byte to the chosen port.
__attribute__((no_caller_saved_registers)) 
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

// Writes a word to the chosen port.
__attribute__((no_caller_saved_registers)) 
static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ( "outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}


// Reads the data from the provided port. This function is just a wrapper to not overuse too much assembly.
__attribute__((no_caller_saved_registers)) 
static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ( "inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
    return val;
}

// Loads the IDT.
static inline void lidt(void *base) {
    // Creating a pointer for lidt instruction
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { .length = sizeof(IDT) - 1, base };
 
    __asm__ ( "lidt %0" : : "m"(IDTR) );
}

/* 
 * Performs a shutdown
 *
 * Or a "shutdown", since the qemu is being used as a primary testing environment, while debugging
 * the shutting down routine will be much easier.
 * */
__attribute__((no_caller_saved_registers)) 
static inline void shutdown() {
    #if !RELEASE 
        outw(0xB004, 0x2000); // Makes QEMU to close, will not execute in release. 
    #else
        
    #endif
}
