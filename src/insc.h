/* 
 *  Additional module for 'daemon' intrinsics functions and datatypes
 * */

/* Static global application timer */
#include <stdint.h>

#define UNSELECTED 0xff

// ID of the selected OS from the OS list. Value '0xff' is considered as not selected yet.
static volatile uint8_t os_id = UNSELECTED;

// Timer that is globally available across the bootloader. Ticks on every PIC's clock tick.
static volatile union {
    uint64_t bits;
} GLOBAL_TIMER;
