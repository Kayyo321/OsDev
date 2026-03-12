; Print a null-terminated string
; Input: bx = pointer to string
print_string:
    pusha
    mov ah, 0x0e        ; BIOS teletype mode
.loop:
    mov al, [bx]
    cmp al, 0
    je .done
    int 0x10
    inc bx
    jmp .loop
.done:
    popa
    ret
