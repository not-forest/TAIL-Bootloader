/*  
    A memory map header that contains info about memory regions of the
    bootloader and the corresponding app.
*/
#pragma once

#ifndef _BOOTLOADER_MEMORY_MAP
#define _BOOTLOADER_MEMORY_MAP

extern int _bootrom_head_start_;
extern int _bootrom_head_size_;
extern int _bootrom_tail_start_;
extern int _bootrom_tail_size_;

#endif