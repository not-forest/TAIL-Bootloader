/*
    A main entry of the TAIL bootloader.
*/

#include<inttypes.h>
#include "memory_map.h"

void main(uint16_t boot_drive) {
    char* c = (char*)0xb8000;
    *c = 'A';

    while(1) {}
}