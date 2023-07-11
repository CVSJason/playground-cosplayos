OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(__cosplay_main)

SECTIONS 
{
    . = 0x0;
    .text (. + 0x1000) : { *(.text) }
    . = ALIGN(0x1000);

    .data : { *(.data .rodata* .bss) }
}