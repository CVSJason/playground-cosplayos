#pragma once

extern "C" void io_hlt();
extern "C" void io_cli();
extern "C" void io_sti();
extern "C" void io_stihlt();
extern "C" void io_out8(int port, int data);
extern "C" void io_out16(int port, int data);
extern "C" void io_out32(int port, int data);
extern "C" int io_in8(int port);
extern "C" int io_in16(int port);
extern "C" int io_in32(int port);
extern "C" int io_load_eflags();
extern "C" void io_store_eflags(int eflags);
extern "C" void load_gdtr(int limit, void *addr);
extern "C" void load_idtr(int limit, void *addr);
extern "C" int load_cr0();
extern "C" void store_cr0(int cr0);
extern "C" void load_tr(int tr);
extern "C" void jumpFar(int eip, int cs);
extern "C" void callFar(int eip, int cs);
extern "C" void startApp(int eip, int cs, int esp, int ds, int *tssEsp0);