;       General purpose assembly function for TAIL
;
;   All defined functions within this module serve only as
;   convenient low level assembly bridge to not overruse C
;   inline assembly statement. For the sake of smaller binary
;   footprint, those functions are stacked in MBR sector with
;   other assembly.

section .text
    global update_cursor
    global enable_cursor
    global disable_cursor
    global itoa
bits 32

; Updates the VGA cursor location based on a row and column value.
update_cursor:
    ; Inputs:
    ;   rdi:  row (uint8_t)
    ;   rsi:  col (uint8_t)
    ; Outputs: None
    ; Clobbers: dx, al

    ; Calculate the position
    movzx ebx, byte [edi]   ; Load row into ebx
    movzx ecx, byte [esi]   ; Load col into ecx
    mov ax, 0x07            ; Load attribute
    mul cx                  ; Multiply row by BUFFER_WIDTH
    add bx, ax              ; Add col
    mov dx, 0x03d4          ; DX = VGA port
    mov al, 0x0f            ; Low cursor byte command
    out dx, al              ; Send command
    inc dx                  ; DX points to high cursor byte port
    mov al, bl              ; Move low cursor byte
    out dx, al              ; Send byte to port
    dec dx                  ; DX points back to low cursor byte port
    mov al, 0x0e            ; High cursor byte command
    out dx, al              ; Send command
    inc dx                  ; DX points to high cursor byte port
    mov al, bh              ; Move high cursor byte
    out dx, al              ; Send byte to port
    ret

disable_cursor:
    ; Outputs: None
    ; Clobbers: dx, al
    mov dx, 0x3d4       ; DX = VGA port
    mov al, 0xa         ; Cursor start register
    out dx, al          ; Send command

    inc dx              ; DX points to cursor end register
    mov al, 0x20        ; Cursor end value (disable)
    out dx, al          ; Send byte to port

    ret

enable_cursor:
    ; Inputs:
    ;   cursor_start (uint8_t)
    ;   cursor_end (uint8_t)
    ; Outputs: None
    ; Clobbers: dx, al

    ; Setting up the cursor's start
    mov dx, 0x3d4       ; DX = VGA port
    mov al, 0xa         ; Cursor start register
    out dx, al          ; Send command

    inc dx              ; DX points to cursor start data port
    in al, dx           ; Read current cursor start value
    and al, 0xc0        ; Clear the cursor start bits
    or al, dl           ; Set the cursor start bits with cursor_start value
    out dx, al          ; Write updated cursor start value

    ; Setting up the cursor's end
    mov dx, 0x3d4       ; DX = VGA port
    mov al, 0xb         ; Cursor end register
    out dx, al          ; Send command

    inc dx              ; DX points to cursor end data port
    in al, dx           ; Read current cursor end value
    and al, 0xe0        ; Clear the cursor end bits
    or al, al           ; Set the cursor end bits with cursor_end value
    out dx, al          ; Write updated cursor end value

    ret

section .rodata

;
; Conversion table for __itoa.
; Works for bases [2 ... 36].
;
__itoacvt:
    db '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

section .text

; Convenience wrapper for __itoa.
; Takes two arguments from the C call: the integer to convert and the base.
; Returns a pointer to the null-terminated string stored in __itoabuf32.
;
; Parameters:
;   eax -> Integer to convert (int n)
;   ecx -> Base (e.g. 10 for decimal, 16 for hex)
;
itoa:
    push ebx

    ; Move arguments to the appropriate registers
    mov dword eax, [esp + 8] ; Load the integer to convert (n)
    mov dword ecx, [esp + 12] ; Load the base (e.g., 10 for decimal)
    mov dword ebx, __itoabuf32 ; Set the destination buffer to __itoabuf32

    call __itoa         ; Call the internal itoa conversion

    mov eax, __itoabuf32 ; Return the pointer to the buffer

    pop ebx
    ret

;
; Routine to convert a 32-bit integer to a string.
; Registers are preserved.
;
; EAX: Source integer
; EBX: Target address
; ECX: Base
;
__itoa:
.start:
    push eax
    push ebx
    push ecx
    push edx
    mov edx, ecx
    mov ecx, ebx
.checknegative:
    test eax, eax
    jns .divrem
    mov byte [ecx], 0x2D
    inc ecx
    mov ebx, ecx
    neg eax
.divrem:
    push edx
    push ecx
    mov ecx, edx
    xor edx, edx
    div ecx
    mov byte dl, [__itoacvt + edx]
    pop ecx
    mov byte [ecx], dl
    pop edx
    inc ecx
    cmp eax, 0x00
    jne .divrem
    mov byte [ecx], 0x00
    dec ecx
.reverse:
    cmp ebx, ecx
    jge .end
    mov byte dl, [ebx]
    mov byte al, [ecx]
    mov byte [ebx], al
    mov byte [ecx], dl
    inc ebx
    dec ecx
    jmp .reverse
.end:
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
   
section .bss

;
; Buffer to store the result of __itoa in.
;
align 64
__itoabuf32:
    resb 36
