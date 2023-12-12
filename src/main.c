/*
    A main entry of the TAIL bootloader.
*/

#include<inttypes.h>

#include "memory_map.h"
#include "vga/vga.h"

void main(uint16_t boot_drive) {
    VGABuffer vga = {.row = 0, .col = 0};
    
    prints("Hello bootloader!!!", COLOR_WHITE, COLOR_BLACK, &vga);

    while(1) {}
}