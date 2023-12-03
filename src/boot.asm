extern main

section .text
    bits 16

init:
    mov al, "o"
    mov ah, 0x0E
    int 0x10

    jmp main
