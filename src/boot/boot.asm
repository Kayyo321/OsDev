; The 512b boot sector

[org 0x7c00]

mov [BootDrive], dl ; BIOS stores our boot drive in dl, best to save it for later

mov bp, 0x8000 ; Move the stack over to 0x8000, way out of the way
mov sp, bp

mov bx, 0x9000 ; Load 
mov dh, 5
mov dl, BootDrive
call disk_load

mov dx, [0x9000]
call print_hex

mov dx, [0x9000 + 512]
call print_hex

jmp $ ; hang

; Globls...

%include "print_hex.asm"
%include "print_string.asm"
%include "disk_load.asm"

BootDrive: db 0

times 510-($-$$) db 0 ; Boot secotr padding
dw 0xaa55             ; Magic boot sector number for BIOS

; We know that BIOS will load only the first 512--byte sector from the disk,
; so if we purposely add a few more sectors to our code by repeating some
; familiar numbers, we can prove to ourselfs that we actually loaded those
; additional two sectors from the disk we booted from.
times 256 dw 0 xdada
times 256 dw 0 xface