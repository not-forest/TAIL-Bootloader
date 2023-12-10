;           Here we define the GDT and set it up properly
;
;   The simplest flat memory model for the second stage of the bootloader.
;   The second stage (TAIL) will make different tweaks later based on the
;   required settings by the loaded kernel.

section .text
bits 16
; NULL segment descriptor
_gdt_null:
    dq 0x0

; Code segment descriptor
_gdt_code:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; Data segment descriptor
_gdt_data:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10010010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

_gdt_end:
    
; GDT descriptor
_gdt_descriptor:
    dw _gdt_end - _gdt_null - 1 ; size (16 bit)
    dd _gdt_null                ; address (32 bit)

_CODE_SEG_ equ _gdt_code - _gdt_null
_DATA_SEG_ equ _gdt_data - _gdt_null
