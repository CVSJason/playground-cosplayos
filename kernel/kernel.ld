OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS 
{
    . = 0x0;
    .text : { *(.text) }
    . = ALIGN(0x1000);

    _load_pos = . + 0x280000; /* Haribote OS 装载的位置。由于 DS 段必须为 1，所以数据装载的位置也要向后移动。*/

    .data _load_pos : { *(.data .rodata* .bss) }
}