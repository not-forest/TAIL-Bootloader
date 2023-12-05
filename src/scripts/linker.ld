/* This linker script provides necessary sections for bootloader. */

ENTRY(main);

SECTIONS
{
    . = 0x7C00;

    .text : AT(0x7C00)
    {
        *(.text*)
        *(.rodata*)
    }

    .signature : AT(0x7DFE)
    {
        /* The boot signature needed by BIOS. */
        SHORT(0xaa55);
    }
}

