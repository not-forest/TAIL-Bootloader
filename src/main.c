/*
    A main entry of the TAIL bootloader.
*/

void main(void) {
    asm (
        "movb $'K', %al\n"
        "movb $0x0e, %ah\n"
        "int $0x10\n"
    );
}