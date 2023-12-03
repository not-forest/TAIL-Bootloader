/*
    A main entry of the TAIL bootloader.
*/

void main(void) {
    __asm__ __volatile__ ("movb $'K', %al\n");
    __asm__ __volatile__ ("movb $0x0e, %ah\n");
    __asm__ __volatile__ ("int $0x10\n");
}