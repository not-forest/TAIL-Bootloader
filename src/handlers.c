/* 
 *  This module defines all interrupt handler function needed for proper
 *  daemon execution. Those handler functions are primarly implemented to
 *  communicate with the backend on mobile side.
 *
 *  All defined functions from this module are automatically linked to IDT
 *  defined in assembly initialization part.
 * */

#include "arch/cpu.h"
#include "arch/pic.h"
#include "insc.h"
#include "vga.h"

#include <stdint.h>

extern char* itoa(int, int); // From insc.asm

extern VGABuffer LOGGER;

/* Halts the whole application completely. */
void GENERAL_HANDLER(struct Iframe *frame) {
#if !__RELEASE__
    println("Error general handler invoked!", L_ERROR, &LOGGER);
#endif
    // Debugging logic... 

    __asm__ volatile ("cli; hlt");
}

#if !__RELEASE__
uint8_t dbg_cnt = 0;
uint8_t dbg_halt = 0;

/* Debug handler. (Debug build only)
 *
 * Writes current breakpoint's number and continues program on next keyboard interrupt
 * */
void BREAKPOINT_HANDLER(struct Iframe *frame) {
    prints("Breakpoint № : ", L_INFO, &LOGGER);
    println(itoa(dbg_cnt, 10), L_INFO, &LOGGER);
    
    dbg_halt = 1;
    while (dbg_halt);
}
#endif

#if _DEF_TIMEOUT_ != 0
uint8_t timer_num_places(uint16_t val) {
    if (val < 10) return 0;
    if (val < 100) return 1;
    if (val < 10000) return 2;
    __builtin_unreachable();
}
#endif

/* Handles an event that is being called on each PIC's clock tick. */
void SOFTWARE_TIMER_HANDLER(struct Iframe *frame) { 
    ++GLOBAL_TIMER.bits;


// Timeout that defines delay before the first listed OS will be automatically picked. Zero means no timeout.
#if _DEF_TIMEOUT_ != 0
    if (_DEF_TIMEOUT_ && GLOBAL_TIMER.bits > _DEF_TIMEOUT_)
        os_id = 0;  // Selecting the first entry on timeout.

    // Getting current cursor's location and writing updated time.
    uint8_t row = LOGGER.row, col = LOGGER.row;
    uint16_t time_left = _DEF_TIMEOUT_ - GLOBAL_TIMER.bits;

    *(uint64_t *)(BUFFER_PTR + BUFFER_WIDTH * 4 - 10) = 0;

    LOGGER = (VGABuffer) { .row = 1, .col = BUFFER_WIDTH - 3 - timer_num_places(time_left) };
    prints(itoa(time_left, 10), COLOR_MAGENTA, &LOGGER);
    LOGGER = (VGABuffer) { .row = row, .col = col };
#endif

    end_of_interrupt(PIC1_COMMAND);
}

void SOFTWARE_KEYBOARD_HANDLER(struct Iframe *frame) {
    // IO logic...
    
#if !__RELEASE__
    dbg_halt = 0;
#endif

    end_of_interrupt(PIC1_COMMAND);
}

// Aliasing out undefined handlers to GENERAL_HANDLER.
#define __aliased(name) \
    void name(struct Iframe *frame) __attribute__((alias("GENERAL_HANDLER")));

/* All defined handles are commented out */

__aliased(DIVISION_ERROR_HANDLER);
__aliased(DEBUG_HANDLER);
__aliased(NMI_HANDLER);

#if __RELEASE__
__aliased(BREAKPOINT_HANDLER);
#endif

__aliased(OVERFLOW_HANDLER);
__aliased(BRE_HANDLER);
__aliased(INVALID_OPCODE_HANDLER);
__aliased(DEVICE_NOT_AVAILABLE_HANDLER);
__aliased(DOUBLE_FAULT_HANDLER);
__aliased(INVALID_TSS_HANDLER);
__aliased(SEGMENT_NOT_PRESENT_HANDLER);
__aliased(STACK_SEGMENT_FAULT_HANDLER);
__aliased(GENERAL_PROTECTION_FAULT_HANDLER);
__aliased(X87_FP_EXCEPTION_HANDLER);
__aliased(ALIGNMENT_CHECK_HANDLER);
__aliased(MACHINE_CHECK_HANDLER);
__aliased(SIMD_FP_EXCEPTION_HANDLER);
__aliased(VIRTUALIZATION_EXCEPTION_HANDLER);
__aliased(CONTROL_PROTECTION_HANDLER);
__aliased(HIE_HANDLER);
__aliased(VMMC_EXCEPTION_HANDLER);
__aliased(SECURITY_EXCEPTION_HANDLER);


//__aliased(SOFTWARE_TIMER_HANDLER);
//__aliased(SOFTWARE_KEYBOARD_HANDLER);
