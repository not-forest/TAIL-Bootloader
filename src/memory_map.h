/*  
    A memory map header that contains info about memory regions of the
    bootloader and the corresponding app.
*/
#pragma once

#ifndef _BOOTLOADER_MEMORY_MAP
#define _BOOTLOADER_MEMORY_MAP

extern int _MBRROM_ADDR_; // Address of the master boot.
extern int _MBRROM_SIZE_; // Size of the master boot.
extern int _APPROM_ADDR_; // Address of the second stage.
extern int _APPROM_SIZE_; // Size of the second stage.
extern int _APPRAM_ADDR_; // Stack address.

#endif