/* 
    A linker script for defining the memory mapping. 

    This memory mapping is defined for both two stages of the bootloader.
*/

MEMORY
{
    mbrrom (rx)  : ORIGIN = 0x7C00, LENGTH = 512 /* First stage  */
    approm (rx)  : ORIGIN = 0x7E00, LENGTH = 20K /* Second stage */
    appram (rwx) : ORIGIN = 0xEC00, LENGTH = 1K /* Second stage */
}

_STACK_SIZE_ = 1K;

_MBRROM_ADDR_ = ABSOLUTE(ORIGIN(mbrrom));
_MBRROM_SIZE_ = ABSOLUTE(LENGTH(mbrrom));
_APPROM_ADDR_ = ABSOLUTE(ORIGIN(approm));
_APPROM_SIZE_ = ABSOLUTE(LENGTH(approm));
_APPRAM_ADDR_ = ABSOLUTE(ORIGIN(appram));