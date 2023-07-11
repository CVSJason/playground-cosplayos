    bits 32

    extern  inthandler0d, inthandler21, inthandler2c, inthandler20, _consoleApi

    global  io_hlt, io_cli, io_sti, io_stihlt
    global  io_in8, io_in16, io_in32
    global  io_out8, io_out16, io_out32
    global  io_load_eflags, io_store_eflags
    global  load_gdtr, load_idtr
    global  load_cr0, store_cr0
    global  load_tr
    global  jumpFar, callFar
    global  asm_inthandler0d, asm_inthandler21, asm_inthandler2c, asm_inthandler20
    global  asm_consoleApi
    global  startApp

section .text

io_hlt:
    hlt
    ret

io_cli:
    cli
    ret

io_sti:
    sti
    ret

io_stihlt:
    sti
    hlt

io_in8:
    mov     edx, [esp + 4]
    mov     eax, 0
    in      al, dx
    ret 

io_in16:
    mov     edx, [esp + 4]
    mov     eax, 0
    in      ax, dx
    ret 

io_in32:
    mov     edx, [esp + 4]
    in      eax, dx
    ret

io_out8:
    mov     edx, [esp + 4]
    mov     al, [esp + 8]
    out     dx, al
    ret

io_out16:
    mov     edx, [esp + 4]
    mov     eax, [esp + 8]
    out     dx, ax
    ret

io_out32:
    mov     edx, [esp + 4]
    mov     eax, [esp + 8]
    out     dx, eax
    ret

io_load_eflags:
    pushfd
    pop     eax
    ret

io_store_eflags:
    mov     eax, [esp + 4]
    push    eax
    popfd
    ret

load_gdtr:
    mov     ax, [esp + 4]
    mov     [esp + 6], ax
    lgdt    [esp + 6]
    ret

load_idtr:
    mov     ax, [esp + 4]
    mov     [esp + 6], ax
    lidt    [esp + 6]
    ret

load_cr0:
    mov     eax, cr0
    ret

store_cr0:
    mov     eax, [esp + 4]
    mov     cr0, eax
    ret

load_tr:
    ltr     [esp + 4]
    ret

jumpFar:
    jmp     far [esp + 4]
    ret

callFar:
    call    far [esp + 4]
    ret

asm_inthandler0d:
    sti
    push    es
    push    ds
    pushad
    mov     eax, esp
    push    eax
    mov     ax, ss
    mov     ds, ax
    mov     es, ax
    call    inthandler0d
    cmp     eax, 0
    jne     .end_app
    pop     eax
    popad
    pop     ds
    pop     es
    add     esp, 4
    iretd
.end_app:
    mov     esp, [eax]
    popad
    ret

asm_inthandler21:
    push    es
    push    ds
    pushad
    mov     eax, esp
    push    eax
    mov     ax, ss
    mov     ds, ax
    mov     es, ax
    call    inthandler21
    pop     eax
    popad
    pop     ds
    pop     es
    iretd

asm_inthandler2c:
    push    es
    push    ds
    pushad
    mov     eax, esp
    push    eax
    mov     ax, ss
    mov     ds, ax
    mov     es, ax
    call    inthandler2c
    pop     eax
    popad
    pop     ds
    pop     es
    iretd

asm_inthandler20:
    push    es
    push    ds
    pushad
    mov     eax, esp
    push    eax
    mov     ax, ss
    mov     ds, ax
    mov     es, ax
    call    inthandler20
    pop     eax
    popad
    pop     ds
    pop     es
    iretd

asm_consoleApi:
    sti
    push    ds
    push    es
    pushad
    pushad
    mov     ax, ss
    mov     ds, ax
    mov     es, ax
    call    _consoleApi
    cmp     eax, 0
    jne     .end_app
    add     esp, 32
    popad
    pop     es
    pop     ds
    iretd
.end_app:
    mov     esp, [eax]
    popad
    ret

startApp:
    pushad
    mov     eax, [esp + 36]
    mov     ecx, [esp + 40]
    mov     edx, [esp + 44]
    mov     ebx, [esp + 48]
    mov     ebp, [esp + 52]
    mov     [ebp    ], esp
    mov     [ebp + 4], ss
    mov     es, bx
    mov     ds, bx
    mov     fs, bx
    mov     gs, bx
    or      ecx, 3
    or      ebx, 3
    push    ebx
    push    edx
    push    ecx
    push    eax
    retf