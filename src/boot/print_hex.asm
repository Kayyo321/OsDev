; Print a 16-bit value as hex (e.g. 0x1A2B)
; Input: dx = value to print
print_hex:
    pusha
    mov cx, 4           ; 4 hex digits
.loop:
    rol dx, 4           ; rotate highest nibble into lowest
    mov al, dl
    and al, 0x0f        ; isolate nibble
    cmp al, 10
    jl .digit
    add al, 'A' - 10 - '0'
.digit:
    add al, '0'
    mov ah, 0x0e
    int 0x10
    dec cx
    jnz .loop
    popa
    ret
