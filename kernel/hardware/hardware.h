#pragma once
#include "../utils.h"

struct SegmentDescriptor {
    short limitLow, baseLow;
    char  baseMid, accessRight;
    char  limitHigh, baseHigh;
};

struct GateDescriptor {
    short offsetLow, selector;
    char dwCount, accessRight;
    short offsetHigh;
};

struct BootInfo {
    byte cylinders, leds, vmode, reserve;
    short screenWidth, screenHeight;
    byte *vram;
};

struct TSS32 {
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    int es, cs, ss, ds, fs, gs;
    int ldtr, iomap;
};

#define MAX_TASKS      1000
#define MAX_TASKS_IN_LEVEL 100
#define TASK_GDT_START 3
#define MAX_TASK_LEVELS 10

struct Task {
    int gdtId, flags;
    TSS32 tss;
    int priority, level;
    void *taskController;
    UIntQueue *queue;
};

struct TaskLevel {
    int count;
    int current;
    Task *tasks[MAX_TASKS_IN_LEVEL];
};

struct MouseData {
    char buffer[3];
    int phase;
    int x, y, button;
};

struct MemoryManagerEntry {
    void *address;
    uint size;
};

class MemoryManager {
public:
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager() {}

    uint calculateFree();
    void *allocate(uint size);
    int release(void *addr, uint size);

    void *allocatePageAlign(uint size) {
        size = (size + 0xfff) & 0xfffff000;

        return allocate(size);
    }

    int releasePageAlign(void *addr, uint size) {
        size = (size + 0xfff) & 0xfffff000;

        return release(addr, size);
    }
private:
    int entryCount = 0, entryCountMax = 0, lostSize = 0, lostCount = 0;
    MemoryManagerEntry entries[4090];
};

#define MAX_TIMER 500
#define TIMER_FLAGS_USING   1
#define TIMER_FLAGS_IDLE    2

struct Timer {
    uint timeoutTime, flags, dataToSend;
    UIntQueue *queue;
    Timer *nextTimer;
    void *timerController;
    Task *task;

    void release();
    void reset(uint timeoutTime, uint dataToSend);
    void cancel();
};

class TimerController {
    friend Timer;

public:
    TimerController(const TimerController&) = delete;
    TimerController(): timer(0) {
        timers[0].flags = TIMER_FLAGS_USING;
        timers[0].timeoutTime = 0xffffffff;

        nextTimer = timers + 0;
    }

    Timer *newTimer(UIntQueue *ByteQueue, uint timeoutTime, uint dataToSend, Task *task = nullptr);
    void proceed();
    uint current() { return timer; }
    void removeTimersAssociatedWithTask(Task *);

private:
    void addTimer(Timer *timer);
    uint timer;
    Timer timers[MAX_TIMER], *nextTimer;
};

class TaskController {
    friend void taskSwitch();

public:
    TaskController(const TaskController &) = delete;
    TaskController() {}

    Task *init(TimerController *controller);
    Task *newTask();
    void run(Task *task, int priority = 0, int level = -1);
    void sleep(Task *task);
    void addTaskToLevel(Task *task);
    void removeTaskFromLevel(Task *task);
    Task *currentTask();
    void switchLevel();

private:
    int currentLevel;
    bool toChangeLevel;
    TaskLevel taskLevels[MAX_TASK_LEVELS];
    Task taskBuffer[MAX_TASKS];
};

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define AR_INTGATE32	0x008e
#define AR_TSS32        0x0089
#define AR_CODE32_ER    0x409a
#define AR_DATA32_RW	0x4092

#define PORT_KEYDATA            0x0060
#define PORT_KEYSTATUS          0x0064
#define PORT_KEYCOMMAND         0x0064
#define KEYSTATUS_NOTREADY      0x02
#define KEYCOMMAND_WRITE_MODE   0x60
#define KEYCOMMAND_LED          0xed
#define KBC_MODE                0x47
#define KEYCOMMAND_SENDTO_MOUSE 0xd4
#define MOUSECOMMAND_ENABLE     0xf4

#define GDT_ADDR 0x00270000
#define IDT_ADDR 0x0026f800

/* tables.cpp */
void setSegmentDescriptor(SegmentDescriptor *descriptor, uint limit, int base, int access);
void setGateDescriptor(GateDescriptor *descriptor, int offset, int selector, int access);
void initGdtAndIdt();

/* interrupt.cpp */
void initPic();

/* input.cpp */
void waitKbcSendReady();
void initKeyboard();
void enableMouse();

/* timer.cpp */
void initPit();

/* memory.cpp */
volatile uint memoryTest(uint start, uint end);

/* task.cpp */
void taskSwitch();

/* asminc.asm */
extern "C" void *asm_inthandler0c();
extern "C" void *asm_inthandler0d();
extern "C" void asm_inthandler21();
extern "C" void asm_inthandler2c();
extern "C" void asm_inthandler20();
extern "C" void asm_consoleApi();