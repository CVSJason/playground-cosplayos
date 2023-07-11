#include "stdio.h"
#include "../utils.h"
#include "hardware.h"
#include "../graphics/graphics.h"
#include "../programs/programs.h"

void initPic() {
    io_out8(PIC0_IMR, 0b11111111);
    io_out8(PIC1_IMR, 0b11111111);

    io_out8(PIC0_ICW1, 0x11);
    io_out8(PIC0_ICW2, 0x20);
    io_out8(PIC0_ICW3, 1 << 2);
    io_out8(PIC0_ICW4, 0x01);

    io_out8(PIC1_ICW1, 0x11);
    io_out8(PIC1_ICW2, 0x28);
    io_out8(PIC1_ICW3, 2);
    io_out8(PIC1_ICW4, 0x01);

    io_out8(PIC0_IMR, 0b11111011);
    io_out8(PIC1_IMR, 0b11111111);
}

extern UIntQueue *ioQueue;

extern "C" void inthandler21(int *esp) {
    io_out8(PIC0_OCW2, 0x61);

    ioQueue->push(io_in8(0x0060), IOQUEUE_KEYBOARD_IN);
}

extern "C" void inthandler2c(int *esp) {
    io_out8(PIC1_OCW2, 0x64);
    io_out8(PIC0_OCW2, 0x62);
    
    ioQueue->push(io_in8(PORT_KEYDATA), IOQUEUE_MOUSE_IN);
}

extern TimerController *timerController;

extern "C" void inthandler20(int *esp) {
    io_out8(PIC0_OCW2, 0x60);
    
    io_cli();
    timerController->proceed();
    io_sti();
}

extern TaskController *taskController;

extern "C" void *inthandler0d(int *esp) {
    let consoleData = *(ConsoleData **)0x0fec;
    let task = taskController->currentTask();

    void _consolePutString(ConsoleData *consoleData, const char *str);

    _consolePutString(consoleData, "\nThe app was stopped because of general protected exception.");    

    return &(task->tss.esp0);
}

extern "C" void *inthandler0c(int *esp) {
    let consoleData = *(ConsoleData **)0x0fec;
    let task = taskController->currentTask();

    void _consolePutString(ConsoleData *consoleData, const char *str);

    _consolePutString(consoleData, "\nThe app was stopped because of stack exception.");    

    return &(task->tss.esp0);
}

