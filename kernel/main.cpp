#include <stdio.h>
#include "utils.h"
#include "graphics/graphics.h"
#include "hardware/hardware.h"
#include "programs/programs.h"

byte cursorTexture[16 * 16];

UIntQueue *ioQueue;
uint ioQueueBuffer[256];

MouseData mouseData { { 0, 0, 0 }, -1, 160, 100, 0 };

MemoryManager *memoryManager;
LayerController *layerController;
TimerController *timerController;
TaskController *taskController;

int totalMemory;

void taskBProgram(Layer *layer) {
    uint count = 0;

    forever {
        for_until(i, 0, 1231) {
            count++;
        }
        {
            char str[12];
            sprintf(str, "%10d", count);
            paintStringToLayerAndRefresh(layer, 4, 28, HARIB_COL_000, 16, str);
        }
    }
}

extern "C" int main() {
    let bootInfo = (BootInfo *) BOOTINFOADDR;

    UIntQueue _ioQueue(ioQueueBuffer, 256, nullptr);
    ioQueue = &_ioQueue;

    memoryManager = (MemoryManager *) 0x003c0000;
    
    *memoryManager = MemoryManager();

    initGdtAndIdt();
    initPic();
    initPalette();

    totalMemory = memoryTest(0x00400000, 0xbfffffff);

    memoryManager->release((void *)0x00001000, 0x0009e000);
    memoryManager->release((void *)0x00400000, totalMemory - 0x00400000);

    timerController = (TimerController *)memoryManager->allocatePageAlign(sizeof (TimerController));

    let taskTimer   = timerController->newTimer(ioQueue, timerController->current() + 2, 2);

    let vram = bootInfo->vram;
    let screenWidth = bootInfo->screenWidth, 
        screenHeight = bootInfo->screenHeight;

    layerController = (LayerController *)memoryManager->allocatePageAlign(sizeof (LayerController));
    layerController->init(memoryManager, vram, screenWidth, screenHeight);

    let backgroundLayer = layerController->newLayer(
        (byte *)memoryManager->allocatePageAlign(screenWidth * screenHeight),
        screenWidth, screenHeight, -1
    );

    drawBackground(backgroundLayer->buffer, screenWidth, screenHeight);
    backgroundLayer->setZIndex(0);

    let mouseLayer = layerController->newLayer(
        cursorTexture, 16, 16, 99
    );

    writeCursorTexture(cursorTexture, 99);
    mouseLayer->setZIndex(2);
    mouseLayer->setPosition(mouseData.x, mouseData.y);

    let windowLayer = layerController->newLayer(
        (byte *)memoryManager->allocatePageAlign(800 * 400),
        800, 400, 99
    );

    drawWindow(windowLayer->buffer, windowLayer->width, windowLayer->height, "gosh", 99, true);
    fillRect(windowLayer->buffer, windowLayer->width, HARIB_COL_000, 1, 24, windowLayer->width - 2, windowLayer->height - 25);
    windowRoundCorner(windowLayer->buffer, windowLayer->width, windowLayer->height, 99);

    windowLayer->setPosition(screenWidth / 2 - windowLayer->width / 2, screenHeight / 2 - windowLayer->height / 2);
    windowLayer->setZIndex(1);

    layerController->refresh(0, 0, screenWidth, screenHeight, true);

    //
    // 初始化任务
    //
    taskController = (TaskController *)memoryManager->allocatePageAlign(sizeof(TaskController));
    let taskA = taskController->init(timerController);
    taskController->run(taskA, 0, 1);
    ioQueue->setTask(taskA);

    let consoleTask = taskController->newTask();
    consoleTask->tss.esp = (int)memoryManager->allocatePageAlign(64 * 1024) + 64 * 1024 - 8;
    consoleTask->tss.eip = (int)::consoleTask;
    consoleTask->tss.es = 1 * 8;
    consoleTask->tss.cs = 2 * 8;
    consoleTask->tss.ss = 1 * 8;
    consoleTask->tss.ds = 1 * 8;
    consoleTask->tss.fs = 1 * 8;
    consoleTask->tss.gs = 1 * 8;
    *((Layer **) (consoleTask->tss.esp + 4)) = windowLayer;
    taskController->run(consoleTask, 2, 2);

    io_sti();
    initKeyboard();
    enableMouse();
    initPit();

    var mousePhase = -1;
    var nextCursorX = 8;
    var consoleIsActive = true;
    var lastKeyboardOut = -1;
    var e0flag = false;
    
    var commandL = false, commandR = false;
    var controlL = false, controlR = false;

    forever {
        io_cli();
        
        bool toRedrawCursor = false;
        bool toRedrawWindow = false;

        while (!ioQueue->isEmpty()) {
            let dataRaw = ioQueue->pop();
            let type = (dataRaw & 0xff000000) >> 24;
            let data = dataRaw & 0x00ffffff;

            if (type == IOQUEUE_KEYBOARD_IN) {
                if (data == 0xe0 && !e0flag) {
                    e0flag = true;
                } else {
                    

                    if (data == 0x0f) { // Tab
                        consoleIsActive = !consoleIsActive;

                        if (consoleIsActive) {
                            drawWindowCaption(windowLayer->buffer, windowLayer->width, windowLayer->height, "gosh", 99, true);
                        } else {
                            drawWindowCaption(windowLayer->buffer, windowLayer->width, windowLayer->height, "gosh", 99, false);
                        }

                        consoleTask->queue->push(consoleIsActive ? 1 : 0, IOQUEUE_CONTROL_IN);
                        
                        layerController->refresh(windowLayer->x, windowLayer->y, windowLayer->width, 24);

                        continue;
                    } else if (data == 0x3a) { // caps
                        bootInfo->leds ^= 4;
                        ioQueue->push(KEYCOMMAND_LED, IOQUEUE_KEYBOARD_OUT);
                        ioQueue->push(bootInfo->leds, IOQUEUE_KEYBOARD_OUT);
                    } else if (data == 0x45) { // num
                        bootInfo->leds ^= 2;
                        ioQueue->push(KEYCOMMAND_LED, IOQUEUE_KEYBOARD_OUT);
                        ioQueue->push(bootInfo->leds, IOQUEUE_KEYBOARD_OUT);
                    } else if (data == 0x46) { // scroll
                        bootInfo->leds ^= 1;
                        ioQueue->push(KEYCOMMAND_LED, IOQUEUE_KEYBOARD_OUT);
                        ioQueue->push(bootInfo->leds, IOQUEUE_KEYBOARD_OUT);
                    } else if (data == 0xfa) {
                        lastKeyboardOut = -1;
                    } else if (data == 0xfe) {
                        waitKbcSendReady();
                        io_out8(PORT_KEYDATA, lastKeyboardOut);
                    } else if (data == 0x5b && e0flag) {
                        commandL = true;
                    } else if (data == 0xdb && e0flag) {
                        commandL = false;
                    } else if (data == 0x5c && e0flag) {
                        commandR = true;
                    } else if (data == 0xdc && e0flag) {
                        commandR = false;
                    } else if (data == 0x1d) {
                        if (e0flag) controlL = true; else controlR = true;
                    } else if (data == 0x9d) {
                        if (e0flag) controlL = false; else controlR = false;
                    } else {
                        if ((controlL || controlR) && data == 0x2e) {
                            if (consoleTask->tss.ss0 != 0) {
                                let console = *(ConsoleData **)0x0fec;

                                void _consolePutString(ConsoleData *consoleData, const char *str);

                                _consolePutString(console, "^C\n");

                                io_cli();

                                consoleTask->tss.eax = (int) &(consoleTask->tss.esp0);
                                consoleTask->tss.eip = (int) endApp;
                                
                                io_sti();
                            }

                            e0flag = false;
                            continue;
                        }
                    }

                    e0flag = false;
                }

                if (consoleIsActive) {
                    consoleTask->queue->push(data, type);
                }
            } else if (type == IOQUEUE_MOUSE_IN) {
                if (mousePhase == -1) {
                    if (data == 0xfa) mousePhase++;

                    continue;
                }

                if (mousePhase >= 0 && (mousePhase != 0 || (data & 0xc8) == 0x08)) 
                    mouseData.buffer[mousePhase++] = (byte)data;

                if (mousePhase == 3) {
                    mousePhase = 0;

                    mouseData.button = mouseData.buffer[0] & 0x07;
                    
                    var xDelta = mouseData.buffer[1], yDelta = mouseData.buffer[2];

                    if ((mouseData.buffer[0] & 0x10) != 0) {
                        xDelta |= 0xffffff00;
                    }

                    if ((mouseData.buffer[0] & 0x20) != 0) {
                        yDelta |= 0xffffff00;
                    }

                    yDelta = -yDelta;

                    mouseData.x += xDelta;
                    mouseData.y += yDelta;

                    if (mouseData.x < 0) mouseData.x = 0;
                    if (mouseData.y < 0) mouseData.y = 0;
                    if (mouseData.x >= screenWidth) mouseData.x = screenWidth - 1;
                    if (mouseData.y > screenHeight) mouseData.y = screenHeight;

                    if (mouseData.button & 0x01) {
                        toRedrawWindow = true;
                    }

                    toRedrawCursor = true;
                }
            } else if (type == IOQUEUE_TIMER_IN) {
                
            } else if (type == IOQUEUE_KEYBOARD_OUT) {
                lastKeyboardOut = data & 0xff;
                waitKbcSendReady();
                io_out8(PORT_KEYDATA, lastKeyboardOut);
            }
        }

        if (toRedrawCursor) {
            io_sti();

            mouseLayer->setPosition(mouseData.x, mouseData.y);
        }

        if (toRedrawWindow) {
            io_sti();

            windowLayer->setPosition(mouseData.x - windowLayer->width / 2, mouseData.y - 12);
        }

        io_sti();

        taskController->sleep(taskA);
    }
}