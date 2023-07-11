#include "utils.h"
#include "hardware/hardware.h"

int UIntQueue::push(uint b, byte type) {
    if (count >= capacity) return -1;

    io_cli();
    data[writePos++] = (b & 0x00ffffff) | ((uint)type << 24);

    if (writePos == capacity) writePos = 0;

    count++;

    if (task != nullptr && ((Task *)task)->flags != 2) {
        io_sti();
        ((TaskController *)((Task *)task)->taskController)->run((Task*)task);
    }

    io_sti();

    return 0;
}

uint UIntQueue::pop() {
    if (count == 0) {
        return 255;
    }

    io_cli();
    let result = data[readPos++];

    if (readPos == capacity) {
        readPos = 0;
    }

    count--;
    io_sti();

    return result;
}

extern "C" void *memcpy(void *dest, const void *src, size_t count) {
    var i = 0u;

    for (; i < count - 3; i += 4) {
        *(uint*)((uint)src + i) = *(uint*)((uint)dest + i);
    }

    for (; i < count; ++i) {
        *(byte*)((uint)src + i) = *(byte*)((uint)dest + i);
    }

    return dest;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}