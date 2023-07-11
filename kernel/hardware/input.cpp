#include "../utils.h"
#include "hardware.h"

void waitKbcSendReady() {
    while (io_in8(PORT_KEYSTATUS) & KEYSTATUS_NOTREADY);
}

void initKeyboard() {
    io_out8(PIC0_IMR, 0xf8); 
	io_out8(PIC1_IMR, 0xef); 

    waitKbcSendReady();
    io_out8(PORT_KEYCOMMAND, KEYCOMMAND_WRITE_MODE);
    waitKbcSendReady();
    io_out8(PORT_KEYDATA, KBC_MODE);
}

void enableMouse() {
    waitKbcSendReady();
    io_out8(PORT_KEYCOMMAND, KEYCOMMAND_SENDTO_MOUSE);
    waitKbcSendReady();
    io_out8(PORT_KEYDATA, MOUSECOMMAND_ENABLE);
}