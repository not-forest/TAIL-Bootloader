/*
    A main entry of the TAIL bootloader.
*/

#include<inttypes.h>

#include "memory_map.h"
#include "vga/vga.h"

void main(uint16_t boot_drive) {
    VGABuffer vga = {.row = 0, .col = 0};
    println("Printing the line.", COLOR_WHITE, COLOR_BLACK, &vga);

    prints("Hello bootloader!!!.\nThe new line. ", COLOR_WHITE, COLOR_BLACK, &vga);
    prints("Hello again!!!.\0Not this time(", COLOR_WHITE, COLOR_BLACK, &vga);
    vga_swap(2, 1);

    while(1) {}
}