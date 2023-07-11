bits 32

global  main

section .text

main:
    mov     edx, 2
    mov     ebx, message
    int     0x40
    ret

section .data
message:
    db      "Hello, world!"
    db      0