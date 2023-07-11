cpu 486
bits 32

    extern  main

    global  __cosplay_main, apiPutCharUtf32

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