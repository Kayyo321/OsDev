; Switch from 16-bit real mode to 32-bit protected mode

[bits 16]

switch_to_pm:
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load the GDT descriptor

    mov eax, cr0
    or eax, 0x1            ; Set PE (Protection Enable) bit in CR0
    mov cr0, eax

    jmp CODE_SEG:init_pm   ; Far jump to flush the CPU pipeline

[bits 32]

init_pm:
    mov ax, DATA_SEG        ; Update all segment registers to point to the data segment
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000        ; Set up the stack at the top of free space
    mov esp, ebp

    jmp begin_pm            ; Jump to our 32-bit code entry point
