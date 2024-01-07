/* 
    This linker script provides necessary sections for bootloader.
    
    The first stage of the bootloader, which is a small assembly part with
    the purpose of loading the second part.

    The second stage of the bootloader will work in protected mode as well
    as compiled in C, while the MBR is fully assembly part.    
*/
ENTRY(_start)

INCLUDE memory.ld

INPUT(mbr.o gdt.o disk.o main.o)

SECTIONS
{
    . = ORIGIN(mbrrom);

    /* 
        Assembly code that acts as a first stage does the primary steps to 
        start the second stage of the bootloader. This code will be located
        at the mbrrom address.
    */
    .text : AT(ORIGIN(mbrrom))
    {
        mbr.o*(.text)
        gdt.o*(.text)
        disk.o(.text)
    } > mbrrom

    .signature : AT(0x7DFE)
    {
        /* The boot signature needed by BIOS. */
        SHORT(0xaa55)
    }

    /* C code that provides the code needed to load the kernel itself. */
    .text : AT(ORIGIN(approm))
    {
        KEEP(*(.vectors*))
        *(.text*)
        *(.rodata*)
        *(.got*)
    } > approm

    /* Uninit data */
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    } > approm

    /* Initialized data */
    .data :
    {
        *(.data*)
    } > approm

    /* App's start */
    .stack (NOLOAD) : AT(ORIGIN(appram))
    {
        . =  ALIGN(8);
        . += _STACK_SIZE_;
        . =  ALIGN(8);
        _BOOTLOADER_STACK_ = .;
    } > appram

    _BOOTLOADER_END_ = .;
}

