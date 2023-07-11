CYLS  equ 0x0ff0
LEDS  equ 0x0ff1
VMODE equ 0x0ff2
SCRNX equ 0x0ff4
SCRNY equ 0x0ff6
VRAM  equ 0x0ff8

VMODEVAL equ 0x105

DATADST equ 0x00280000
CACHE   equ 0x00100000
CACHE_R equ 0x00008000

    org     0xc200

    mov     ah, 0x02
    int     0x16
    mov     [LEDS], al

    mov     ax, 0x9000
    mov     es, ax
    mov     di, 0
    mov     ax, 0x4f00
    int     0x10
    cmp     ax, 0x004f
    jne     screen320

    mov     ax, [es:di + 4]
    cmp     ax, 0x0200
    jb      screen320

    mov     cx, VMODEVAL
    mov     ax, 0x4f01
    int     0x10
    cmp     ax, 0x004f
    jne     screen320

    cmp     byte [es:di + 0x19], 8
    jne     screen320
    cmp     byte [es:di + 0x1b], 4
    jne     screen320
    mov     ax, [es:di + 0x00]
    and     ax, 0x0080
    jz      screen320

    mov     bx, VMODEVAL + 0x4000
    mov     ax, 0x4f02
    int     0x10
    mov     byte [VMODE], 8
    mov     ax, [ES:DI + 0x12]
    mov     [SCRNX], ax
    mov     ax, [ES:DI + 0x14]
    mov     [SCRNY], ax
    mov     eax, [ES:DI + 0x28]
    mov     [VRAM], eax
    jmp     bootSequence

screen320:
    mov     ah, 0x13
    mov     al, 0x00
    int     0x10

    mov     byte [VMODE], 8
    mov     word [SCRNX], 320
    mov     word [SCRNY], 200
    mov     dword [VRAM], 0x000a0000

bootSequence:
    ; 在进入保护模式前设置程序入口点
    mov     eax, dword [osentry + 0x18]
    mov     dword [jumpcmd + 2], eax

    mov     al, 0xff
    out     0x21, al
    nop
    out     0x21, al
    
    cli

    call    waitKeyboardOut
    mov     al, 0xd1
    out     0x64, al
    call    waitKeyboardOut
    mov     al, 0xdf
    out     0x60, al
    call    waitKeyboardOut 

cpu 486
    lgdt    [gdtr0]
    mov     eax, cr0
    and     eax, 0x7fffffff
    or      eax, 0x00000001
    mov     cr0, eax
    jmp     pipelineFlush

pipelineFlush:
    mov     ax, 1 * 8
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    mov     esi, osentry + 0x1000
    mov     edi, DATADST
    mov     ecx, 512 * 1024 / 4
    call    memcpy

    mov     esi, 0x7c00
    mov     edi, CACHE
    mov     ecx, 512 / 4
    call    memcpy

    mov     esi, CACHE_R + 512
    mov     edi, CACHE + 512
    mov     ecx, 0
    mov     cl, byte [CYLS]
    imul    ecx, 512 * 18 * 2 / 4
    sub     ecx, 512 / 4
    call    memcpy

    mov     esp, dword 0x00310000
    mov     ebp, dword 0x00310000
jumpcmd:
    jmp     dword 2 * 8: 0x00000000

waitKeyboardOut:
    in      al, 0x64
    and     al, 0x02
    in      al, 0x60
    jnz     waitKeyboardOut
    ret

memcpy:
    mov     eax, [esi]
    add     esi, 4
    mov     [edi], eax
    add     edi, 4
    sub     ecx, 1
    jnz     memcpy
    ret

    alignb  16

gdt0:
    resb    8
    dw      0xffff, 0x0000, 0x9200, 0x00cf
    dw      0xffff, 0x0000, 0x9a28, 0x0047

    dw      0

gdtr0:
    dw      8 * 3 - 1
    dd      gdt0

    alignb  16

osentry: