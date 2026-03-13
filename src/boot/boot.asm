; 512b boot sector

[org 0x7c00]

KERNEL_OFFSET equ 0x1000

; Save boot drive number (BIOS passes it in dl)
mov [BOOT_DRIVE], dl

; Set the stack.
mov bp, 0x9000
mov sp, bp

mov bx, MsgRealMode
call print_string

call load_kernel      ; Load kernel from disk while still in real mode
call switch_to_pm     ; Switch to protected mode (never returns)

jmp $

; Load kernel sectors from disk into memory at KERNEL_OFFSET
load_kernel:
    mov bx, MsgLoadKernel
    call print_string

    mov bx, KERNEL_OFFSET ; Destination address for kernel
    mov dh, 15            ; Number of sectors to read
    mov dl, [BOOT_DRIVE]  ; Drive number

    mov ah, 0x02          ; BIOS read sector function
    mov al, dh            ; Number of sectors to read
    mov ch, 0x00          ; Cylinder 0
    mov cl, 0x02          ; Start from sector 2 (sector after boot sector)
    mov dh, 0x00          ; Head 0

    int 0x13              ; BIOS disk interrupt
    jc disk_error         ; Jump if carry flag set (error)

    ret

disk_error:
    mov bx, MsgDiskError
    call print_string
    jmp $

; Boot drive number saved by BIOS in dl
BOOT_DRIVE: db 0

%include "print_string.asm"
%include "print_string_pm.asm"
%include "switch_to_pm.asm"
%include "gdt.asm"

[bits 32]

begin_pm:
    mov ebx, MsgProtMode
    call print_string_pm

    call KERNEL_OFFSET    ; Jump to the kernel

    jmp $                 ; Hang if kernel returns

MsgRealMode:   db 'Started in 16 bit mode', 13, 10, 0
MsgProtMode:   db 'Entered 32 bit protected mode', 0
MsgLoadKernel: db 'Loading kernel into memory', 13, 10, 0
MsgDiskError:  db 'Disk read error!', 0

times 510-($-$$) db 0
dw 0xaa55 ; Boot sector magic