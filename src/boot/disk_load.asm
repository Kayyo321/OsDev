disk_load:
    push dx

    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02

    int 0x13

    jc disk_error

    pop dx

    cmp dh, al
    jne disk_error 
    
    ret

disk_error:
    mov bx, DiskErrorMsg
    call print_string
    jmp $ ; Hang

%include "print_string.asm"

DiskErrorMsg: db 'Disk Read Error!', 0