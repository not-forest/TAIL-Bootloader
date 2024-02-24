/* 
 * Module for segmentation
 *
 * The bootloader works in protected mode, therefore the segmentation is a required feature
 * and there is no further way of implementing the interrupts without proper GDT setup. This
 * module provides type definitions for configuring out the GDT with proper segments.
 *
 * */
#include <stdint.h>

/* 
 * An entry of the GDT.
 *
 * Each descriptor describes the system segments, such as LDT and TSS.
 * */
typedef struct {
    uint16_t limit_0_15;
    uint16_t base_16_31;
    uint8_t base_32_39;
    uint8_t access_byte;
    uint8_t limit: 4;
    uint8_t flags: 4;
    uint8_t base_56_63;
} SegmentDescriptor;

/* 
 * Binary data structure that contains info about memory segments.
 *
 * While the GDT can be up to 8192 entries, this is not necessary for
 * small bootloaders. Different setups must be used for different OSes,
 * hovewer most of them set this up themself. The first entry must always
 * be zero.
 * */
typedef struct {
    SegmentDescriptor entries[8];
} GDT;
