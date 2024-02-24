/*  
    A memory map header that contains info about memory regions of the
    bootloader and the corresponding app.
*/
#pragma once

#include<stdint.h>

#ifndef _BOOTLOADER_MEMORY_MAP_
#define _BOOTLOADER_MEMORY_MAP_

extern uint32_t _MBRROM_ADDR_;    // Address of the master boot.
extern uint16_t _MBRROM_SIZE_;    // Size of the master boot.
extern uint32_t _APPROM_ADDR_;    // Address of the second stage.
extern uint16_t _APPROM_SIZE_;    // Size of the second stage.
extern uint32_t _APPRAM_ADDR_;    // Stack address.

#endif
