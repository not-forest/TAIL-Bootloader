ENTRY(init);

SECTIONS
{
    . = 0x7C00;

    .text : AT(0x7C00)
    {
        *(.text);
    }

    .signature : AT(0x7DFE)
    {
        SHORT(0xaa55);
    }
}