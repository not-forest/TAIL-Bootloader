/* 
 * Module for interrupting the cpu and call some close to hardware code. 
 * */

#include <inttypes.h>
#include <stdint.h>

// A special type for gate descriptor, which is an entry in the IDT.
typedef uint8_t GateDescriptor;

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
static inline void wfi() {
    asm (
        "sti\n"
        "hlt"
    );
}
