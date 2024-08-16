/* 
    This linker script provides necessary sections for bootloader.
    
    The first stage of the bootloader, which is a small assembly part with
    the purpose of loading the second part.

    The second stage of the bootloader will work in protected mode as well
    as compiled in C, while the MBR is fully assembly part.    
*/

/* Entry point */
ENTRY(_start)

/* Memory areas definitions */
INCLUDE mem.ld

/* Separated assembly imports. */
INPUT(mbr.o gdt.o disk.o idt.o insc.o) 

SECTIONS
{
    . = ORIGIN(mbrrom); /* Starting at MBR address. */

    /* 
        Assembly code that acts as a first stage does the primary steps to 
        start the second stage of the bootloader. This code will be located
        at the mbrrom address.
    */
    .text : AT(ORIGIN(mbrrom))
    {
        /* Assembly */
        mbr.o*(.text)   /* Main first stage functions for initializing the environment. */
        gdt.o*(.text)   /* Setting up the simplest GDT. */
        disk.o*(.text)  /* Reading the bootloader from disk, via BIOS. */

        insc.o*(.text)  /* Bootloader's intrinsics, used from  */
    } > mbrrom

    .signature : AT(0x7DFE)
    {
        /* The boot signature needed by BIOS. */
        SHORT(0xaa55)
    }

    /* Main application space. Starting in the second disk sector. */
    .text : AT(ORIGIN(approm))
    {
        KEEP(*(.vectors*))
        *(.text*)
        *(.rodata*)
        *(.got*)
        /* Amount of sectors to copy from the disk. */
        _SECTORS_AMOUNT_ = ABSOLUTE((. + 511) / 512 - _APPROM_ADDR_);
    } > approm

    /* Uninit data. */
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    } > approm

    /* Initialized data. */
    .data :
    {
        *(.data*)
    } > approm

    /* Bootloader's stack. */
    .stack (NOLOAD) : AT(ORIGIN(appram))
    {
        . =  ALIGN(8);
        . += _STACK_SIZE_;
        . =  ALIGN(8);
        _STACK_TOP_ = .;                        /* Stack top address. Needed for the first stage to set it up. */
    } > appram

    /DISCARD/ : 
    {
        *(COMMON)                               /* Removing all repeated symbols. */
    }

    _BOOTLOADER_END_ = .;                       /* Symbol which defines the end of bootloader. */
}
