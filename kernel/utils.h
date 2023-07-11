#pragma once

#define var auto
#define let const auto
#define forever for(;;)
#define for_to(v,f,s) for (var v = (f); v <= (s); v++)
#define for_until(v,f,s) for (var v = (f); v < (s); v++)

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned int size_t;

#define BOOTINFOADDR  0x00000ff0
#define DISKIMAGEADDR 0x00100000

#define IOQUEUE_KEYBOARD_IN  0x01
#define IOQUEUE_MOUSE_IN     0x02
#define IOQUEUE_TIMER_IN     0x03
#define IOQUEUE_CONTROL_IN   0x04
#define IOQUEUE_KEYBOARD_OUT 0x81

class UIntQueue {
public:
    UIntQueue(const UIntQueue&) = delete;
    UIntQueue(uint *buffer, int capacity, void *task): data(buffer), readPos(0), writePos(0), count(0), capacity(capacity), task(task) {}

    inline bool isEmpty() { return count == 0; }
    
    int push(uint b, byte type);
    uint pop();
    void setTask(void *task) { this->task = task; }

    inline int size() { return count; }
    inline int getCapacity() { return capacity; }
private:
    uint *data;
    int readPos, writePos, count, capacity;
    void *task;
};

int max(int a, int b);
int min(int a, int b);

#include "asmFunc.h"