/*
    A module for interacting with video hardware via BIOS interrupts.
*/
#pragma once

#ifndef _BIOS_VIDEO_SERVISES
#define _BIOS_VIDEO_SERVISES

// Writes a single char to the screen with the use of BIOS.
void inline print_c(const char c) {
    asm volatile (
        "int $0x10" :: "ah"(0x0e00 | c), "al"(0x0007)
    );
}

// Writes the string to the screen with the use of BIOS.
void print_str(const char* str) {
    while(*str) {
        print_c(*str);
        ++str;
    }
}

#endif