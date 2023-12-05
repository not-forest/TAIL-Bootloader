/*
    A main entry of the TAIL bootloader.
*/
asm(".code16gcc");

#include "memory_map.h"
#include "bios/video_servises.h"

void main(void) {
    print_str("Hello bootloader!");

    while(1) {}
}