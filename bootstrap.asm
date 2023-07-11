CYLS equ 40

    org     0x7c00

    jmp     entry
    db      0x90

    db      "Cosplay "
    dw      512
    db      1
    dw      1
    db      2
    dw      224
    dw      2880
    db      0xf0
    dw      9
    dw      18
    dw      2
    dd      0
    dd      2880
    db      0, 0, 0x29
    dd      0xffffffff
    db      "Cosplay OS "
    db      "FAT12   "
    resb    18

entry:
    mov     ax, 0x0820
    mov     es, ax
    mov     ch, 0
    mov     dh, 0
    mov     cl, 2

readLoop:
    mov     si, 0

retry:
    mov     ah, 0x02
    mov     al, 1
    mov     bx, 0
    mov     dl, 0x00
    int     0x13
    jnc     next
    add     si, 1
    cmp     si, 5
    jae     error
    mov     ah, 0x00
    mov     dl, 0x00
    int     0x13
    jmp     retry

next:
    mov     ax, es
    add     ax, 0x0020
    mov     es, ax
    add     cl, 1
    cmp     cl, 18
    jbe     readLoop
    mov     cl, 1
    add     dh, 1
    cmp     dh, 2
    jb      readLoop
    mov     dh, 0
    add     ch, 1
    cmp     ch, CYLS
    jb      readLoop
    jmp     succeed

forever:
    hlt
    jmp     forever

error:
    mov     si, errorMessage
    call    puts
    jmp     forever

succeed:
    mov     byte [0x0ff0], CYLS
    jmp     0xc200

puts:
    mov     ax, 0
    mov     ss, ax
    mov     sp, 0x7c00
    mov     ds, ax
    mov     es, ax

_putsloop:
    mov     al, [si]
    add     si, 1
    cmp     al, 0

    je      forever
    mov     ah, 0x0e
    mov     bx, 15
    int     0x10
    jmp     _putsloop
    ret

errorMessage:
    db      0x0a, 0x0a
    db      "Cosplay: Failed to read files."
    db      0x0a
    db      0

succeedMessage:
    db      0x0a, 0x0a
    db      "Cosplay: Files read successfully."
    db      0x0a
    db      0

    resb    0x1fe-($-$$)
    db      0x55, 0xaa
