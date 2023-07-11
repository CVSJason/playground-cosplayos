cpu 486
bits 32

    extern  main

    global  __cosplay_main, apiPutCharUtf32, apiPutString

section .text

__cosplay_main:
    pushad
    call    main
    popad
    mov     edx, 65536
    int     0x40

apiPutCharUtf32:
    mov     edx, 1
    mov     al, [esp + 4]
    int     0x40
    ret

apiPutString:
    push    ebx
    mov     edx, 2
    mov     ebx, [esp + 8]
    int     0x40
    pop     ebx
    ret