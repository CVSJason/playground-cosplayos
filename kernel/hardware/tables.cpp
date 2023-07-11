#include "../utils.h"
#include "hardware.h"

void setSegmentDescriptor(SegmentDescriptor *descriptor, uint limit, int base, int access) {
    if (limit > 0xffff) {
        access |= 0x8000; // 设置 G bit 为 1
        limit /= 0x1000;
    }

    descriptor->limitLow    = limit & 0xffff;
    descriptor->baseLow     = base & 0xffff;
    descriptor->baseMid     = (base >> 16) & 0xff;
    descriptor->accessRight = access & 0xff;
    descriptor->limitHigh   = ((limit >> 16) & 0x0f) | ((access >> 8) & 0xf0);
    descriptor->baseHigh    = (base >> 24) & 0xff;
}

void setGateDescriptor(GateDescriptor *descriptor, int offset, int selector, int access) {
    descriptor->offsetLow   = offset & 0xffff;
    descriptor->selector    = selector;
    descriptor->dwCount     = (access >> 8) & 0xff;
    descriptor->accessRight = access & 0xff;
    descriptor->offsetHigh  = (offset >> 16) & 0xffff;
}

void initGdtAndIdt() {
    let gdt = (SegmentDescriptor *) GDT_ADDR;
    let idt = (GateDescriptor *)    IDT_ADDR;

    for_until(i, 0, 8192) {
        setSegmentDescriptor(gdt + i, 0, 0, 0);
    }

    setSegmentDescriptor(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
    setSegmentDescriptor(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
    load_gdtr(0xffff, gdt);

    for_until(i, 0, 256) {
        setGateDescriptor(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, idt);
    
    setGateDescriptor(idt + 0x0d, (int)asm_inthandler0d, 2 * 8, AR_INTGATE32);
    setGateDescriptor(idt + 0x21, (int)asm_inthandler21, 2 * 8, AR_INTGATE32);
    setGateDescriptor(idt + 0x2c, (int)asm_inthandler2c, 2 * 8, AR_INTGATE32);
    setGateDescriptor(idt + 0x20, (int)asm_inthandler20, 2 * 8, AR_INTGATE32);
    setGateDescriptor(idt + 0x40, (int)asm_consoleApi,   2 * 8, AR_INTGATE32 + 0x60); // 可供应用程序调用
}