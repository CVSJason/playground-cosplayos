cpu 486
bits 32

    extern  main

    global  __cosplay_main, apiPutCharUtf32, apiPutString
    global  apiOpenWindow, apiPaintString, apiFillRect, apiSetPixel, apiRefresh, apiDrawLine

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

apiOpenWindow:
    push    edi
    push    esi
    push    ebx
    mov     edx, 5
    mov     ebx, [esp + 16]
    mov     esi, [esp + 20]
    mov     edi, [esp + 24]
    mov     eax, [esp + 28]
    mov     ecx, [esp + 32]
    int     0x40
    pop     ebx
    pop     esi
    pop     edi
    ret

apiPaintString:
    push    edi
    push    esi
    push    ebp
    push    ebx
    mov     edx, 6

    mov     ebx, [esp + 20]
    mov     al,  [esp + 40]
    cmp     al, 0
    jne     .cont
    add     ebx, 1
.cont:
    mov     esi, [esp + 24]
    mov     edi, [esp + 28]
    mov     eax, [esp + 32]
    mov     ebp, [esp + 36]

    int     0x40
    pop     ebx
    pop     ebp
    pop     esi
    pop     edi
    ret

apiFillRect:
    push    edi
    push    esi
    push    ebp
    push    ebx
    mov     edx, 7
    mov     ebx, [esp + 20]
    mov     al,  [esp + 44]
    cmp     al, 0
    jne     .cont
    add     ebx, 1
.cont:
    mov     eax, [esp + 24]
    mov     ecx, [esp + 28]
    mov     esi, [esp + 32]
    mov     edi, [esp + 36]
    mov     ebp, [esp + 40]
    int     0x40
    pop     ebx
    pop     ebp
    pop     esi
    pop     edi
    ret

apiSetPixel:
    push    edi
    push    esi
    push    ebx
    mov     edx, 11
    mov     ebx, [esp + 16]
    mov     al,  [esp + 32]
    cmp     al, 0
    jne     .cont
    add     ebx, 1
.cont:
    mov     esi, [esp + 20]
    mov     edi, [esp + 24]
    mov     eax, [esp + 28]
    int     0x40
    pop     ebx
    pop     esi
    pop     edi
    ret

apiRefresh:
    push    edi
    push    esi
    push    ebx
    mov     edx, 12
    mov     ebx, [esp + 16]
    mov     eax, [esp + 20]
    mov     ecx, [esp + 24]
    mov     esi, [esp + 28]
    mov     edi, [esp + 32]
    int     0x40
    pop     ebx
    pop     esi
    pop     edi
    ret

apiDrawLine:
    push    edi
    push    esi
    push    ebp
    push    ebx
    mov     edx, 13
    mov     ebx, [esp + 20]
    mov     al,  [esp + 44]
    cmp     al, 0
    jne     .cont
    add     ebx, 1
.cont:
    mov     eax, [esp + 24]
    mov     ecx, [esp + 28]
    mov     esi, [esp + 32]
    mov     edi, [esp + 36]
    mov     ebp, [esp + 40]
    int     0x40
    pop     ebx
    pop     ebp
    pop     esi
    pop     edi
    ret