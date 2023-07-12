#include "../utils.h"
#include "../graphics/graphics.h"
#include "../hardware/hardware.h"
#include "../fs/fs.h"
#include "programs.h"
#include "stdio.h"
#include "string.h"

extern TaskController *taskController;
extern TimerController *timerController;
extern LayerController *layerController;
extern MemoryManager *memoryManager;

extern int totalMemory;

uint appDSBase;

char keyCodeToLower[0x54] = {
    0  , 0  , '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', 0  , 0  ,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', 0  , 0  , 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0  ,'\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0  , '*',
    0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,
    0  , 0  , 0  , 0  , 0  , 0  , 0  , '7',
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
};

char keyCodeToUpper[0x54] = {
    0  , 0  , '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', 0  , 0  ,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', 0  , 0  , 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0  , '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0  , '*',
    0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,
    0  , 0  , 0  , 0  , 0  , 0  , 0  , '7',
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
};

void _consoleRefreshCursor(ConsoleData *consoleData, bool cursorLit) {
    let layer = consoleData->layer;
    let nextCursorX = consoleData->nextCursorX, nextCursorY = consoleData->nextCursorY;

    fillRect(layer->buffer, layer->width, cursorLit ? HARIB_COL_FFF : HARIB_COL_000, nextCursorX, nextCursorY, 2, 16);
    layerController->refresh(layer->x + nextCursorX, layer->y + nextCursorY, 2, 16);
}

void _consoleLineFeed(ConsoleData *consoleData) {
    let layer = consoleData->layer;

    consoleData->nextCursorY += 16;
    consoleData->nextCursorX = 8;

    if (consoleData->nextCursorY + 16 > layer->height - 8) {
        for (int y = 32; y < layer->height - 24; y++) {
            for (int x = 8; x < layer->width - 8; x++) {
                layer->buffer[y * layer->width + x] = layer->buffer[(y + 16) * layer->width + x];
            }
        }

        fillRect(layer->buffer, layer->width, HARIB_COL_000, 8, layer->height - 24, layer->width - 16, 16);

        layerController->refresh(layer->x + 8, layer->y + 32, layer->width - 16, layer->height - 40);

        consoleData->nextCursorY -= 16;
    }
}

void _consolePutCharUtf32(ConsoleData *consoleData, int chara) {
    let layer = consoleData->layer;

    if (chara == 0x09) {
        while ((consoleData->nextCursorX - 8) / 8 % 4 == 0) {
            consoleData->nextCursorX += 8;
        }

        if (consoleData->nextCursorX >= layer->width - 8) {
            _consoleLineFeed(consoleData);
        }   
    } else if (chara == 0x0a) {
        _consoleLineFeed(consoleData);
    } else if (chara == 0x0d) {
        
    } else {
        paintChar(layer->buffer, layer->width, consoleData->nextCursorX, consoleData->nextCursorY, HARIB_COL_FFF, (char)chara);
        layerController->refresh(layer->x + consoleData->nextCursorX, layer->y + consoleData->nextCursorY, 8, 16);

        consoleData->nextCursorX += 8;

        if (consoleData->nextCursorX >= layer->width - 8) {
            _consoleLineFeed(consoleData);
        }   
    }
} 

char *_consolePutChar(ConsoleData *consoleData, const char* chara) {
    _consolePutCharUtf32(consoleData, *chara);

    return (char*)chara + 1;
}

extern "C" void _consolePutString(ConsoleData *consoleData, const char *str) {
    while (*str != 0) {
        str = _consolePutChar(consoleData, str);
    }
}

bool _consoleCheckCommand(char *command, const char *expected) {
    while (*command != 0 && *expected != 0 && (*command == *expected)) {
        command++; expected++;
    }

    if (*expected == 0 && (*command == 0 || *command == ' ')) return true;
    return false;
}

void _consoleHandleCommand(ConsoleData *consoleData, char *inputBuffer) {
    let fileInfo = (Fat12FileInfo *) (DISKIMAGEADDR + 0x002600);
    let layer = consoleData->layer;

    if (_consoleCheckCommand(inputBuffer, "mem")) {
        char line1[30], line2[30];
        
        sprintf(line1, "Memory: total %dMB", totalMemory / 1024 / 1024);
        sprintf(line2, "        free %dKB", memoryManager->calculateFree() / 1024);

        _consolePutString(consoleData, line1);
        _consoleLineFeed(consoleData);
        _consolePutString(consoleData, line2);
        _consoleLineFeed(consoleData);
        _consoleLineFeed(consoleData);
    } else if (_consoleCheckCommand(inputBuffer, "clear")) {
        fillRect(layer->buffer, layer->width, HARIB_COL_000, 8, 32, layer->width - 16, layer->height - 40);
        layerController->refresh(layer->x + 8, layer->y + 32, layer->width - 16, layer->height - 40);

        consoleData->nextCursorY = 32;
    } else if (_consoleCheckCommand(inputBuffer, "ls")) {
        var lineFed = false;

        for_until(i, 0, 224) {
            if (fileInfo[i].name[0] == 0) break;

            if ((byte)fileInfo[i].name[0] != 0xe5) {
                char str[30];
                var w = str;

                for_until(n, 0, 8) {
                    if (fileInfo[i].name[n] == 0) break;
                    *(w++) = fileInfo[i].name[n];
                }

                if (fileInfo[i].extension[0] != 0) {
                    *(w++) = '.';

                    for_until(n, 0, 3) {
                        if (fileInfo[i].extension[n] == 0) break;
                        *(w++) = fileInfo[i].extension[n];
                    }
                }

                *w = 0;

                let lastCursorX = consoleData->nextCursorX;

                _consolePutString(consoleData, str);

                consoleData->nextCursorX = lastCursorX + 8 * 13;
                if (consoleData->nextCursorX + 8 * 13 > layer->width - 8) {
                    consoleData->nextCursorX = 8;

                    _consoleLineFeed(consoleData);
                    
                    lineFed = true;
                } else {
                    lineFed = false;
                }
            }
        }

        if (!lineFed) _consoleLineFeed(consoleData);
        _consoleLineFeed(consoleData);
    } else if (_consoleCheckCommand(inputBuffer, "cat")) {
        let finfo = getFileInfo(inputBuffer + 4, fileInfo);

        if (finfo == nullptr) {
            _consolePutString(consoleData, "file not found.");
            _consoleLineFeed(consoleData);
        } else {
            let size = finfo->size;
            let data = (byte *) memoryManager->allocatePageAlign(finfo->size);
            loadFileFat12(finfo->clusterId, finfo->size, data, consoleData->fat, (byte *)(DISKIMAGEADDR + 0x003e00));

            for (var j = data; j < data + size;) {
                j = (byte *)_consolePutChar(consoleData, (char*)j);
            }
        
            memoryManager->releasePageAlign(data, finfo->size);
        }

        _consoleLineFeed(consoleData);
    } else if (_consoleCheckCommand(inputBuffer, "readelf")) {
        let finfo = getFileInfo(inputBuffer + 8, fileInfo);

        if (finfo == nullptr) {
            _consolePutString(consoleData, "file does not exist");
            _consoleLineFeed(consoleData);
            _consoleLineFeed(consoleData);

            return;
        } 

        let _codeData = (byte *) memoryManager->allocatePageAlign(finfo->size);
        let codeData = (byte *)(((uint)_codeData + 0xf) & ~0xf);

        appDSBase = (uint)codeData;
        loadFileFat12(finfo->clusterId, finfo->size, codeData, consoleData->fat, (byte *)(DISKIMAGEADDR + 0x003e00));

        if (codeData[0] != 0x7f ||
            codeData[1] != 'E' ||
            codeData[2] != 'L' ||
            codeData[3] != 'F')
        {
            _consolePutString(consoleData, "failed to execute: not a executable file");
            _consoleLineFeed(consoleData);
            _consoleLineFeed(consoleData);
            return;
        }

        let elfHeader = (ELF32Header *)codeData;

        if (elfHeader->bits != 1 || elfHeader->instrSet != 3)
        {
            _consolePutString(consoleData, "failed to execute: can't run on this computer");
            _consoleLineFeed(consoleData);
            _consoleLineFeed(consoleData);
            return;
        }

        let sectionHeaderStart = (byte*)(codeData + elfHeader->sectionHeaderOffset);
        let programHeaderStart = (byte*)(codeData + elfHeader->progHeaderOffset);

        let nameSection = (ELF32SectionHeader*)(sectionHeaderStart + elfHeader->sectionNameStringTableOffset * elfHeader->sectionHeaderSize);

        _consolePutString(consoleData, "                    name    offset     size     addr\n");

        for_until(i, 0, elfHeader->sectionHeaderCount) {
            let sectionHeader = (ELF32SectionHeader*)(sectionHeaderStart + elfHeader->sectionHeaderSize * i);
            let sectionName = (char*)(codeData + nameSection->sectionOffset + sectionHeader->sectionName);

            char str[100];
            
            sprintf(str, "section(%16s) %08x+%08x %08x %s", 
                sectionName,
                sectionHeader->sectionOffset,
                sectionHeader->sectionSize,
                sectionHeader->sectionAddr,
                strcmp(sectionName, ".bss") == 0 ? "(bss)" : ""
            );
            _consolePutString(consoleData, str);
            _consoleLineFeed(consoleData);
        }

        memoryManager->releasePageAlign(_codeData, finfo->size);

        _consoleLineFeed(consoleData);
    } else if (inputBuffer[0] != 0) {
        var args = inputBuffer;

        while (*args != 0 && *args != ' ') args++;

        if (*args == ' ') {
            *args = 0;
            args++;
        }

        let finfo = getFileInfo(inputBuffer, fileInfo);

        if (finfo == nullptr) {
            _consolePutString(consoleData, "command not found.");
            _consoleLineFeed(consoleData);
            _consoleLineFeed(consoleData);

            return;
        } 

        let codeData = (byte *) memoryManager->allocatePageAlign(finfo->size);

        loadFileFat12(finfo->clusterId, finfo->size, codeData, consoleData->fat, (byte *)(DISKIMAGEADDR + 0x003e00));

        if (codeData[0] != 0x7f ||
            codeData[1] != 'E' ||
            codeData[2] != 'L' ||
            codeData[3] != 'F')
        {
            _consolePutString(consoleData, "failed to execute: not a executable file");
            _consoleLineFeed(consoleData);
            _consoleLineFeed(consoleData);
            return;
        }

        let elfHeader = (ELF32Header *)codeData;

        if (elfHeader->bits != 1 || elfHeader->instrSet != 3)
        {
            _consolePutString(consoleData, "failed to execute: can't run on this computer");
            _consoleLineFeed(consoleData);
            _consoleLineFeed(consoleData);
            return;
        }

        let sectionHeaderStart = (byte*)(codeData + elfHeader->sectionHeaderOffset);
        var bssSize = 0;

        let nameSection = (ELF32SectionHeader*)(sectionHeaderStart + elfHeader->sectionNameStringTableOffset * elfHeader->sectionHeaderSize);

        for_until(i, 0, elfHeader->sectionHeaderCount) {
            let sectionHeader = (ELF32SectionHeader*)(sectionHeaderStart + elfHeader->sectionHeaderSize * i);
            let sectionName = (char*)(codeData + nameSection->sectionOffset + sectionHeader->sectionName);

            if (strcmp(sectionName, ".bss") == 0) {
                bssSize = sectionHeader->sectionSize;
            };
        }

        let dataSegmentSize = finfo->size + 64 * 1024 + bssSize;
        let _dataSegment = (byte *) memoryManager->allocatePageAlign(dataSegmentSize + 16);
        let dataSegment = (byte *)(((uint)_dataSegment + 0xf) & ~0xf);

        appDSBase = (uint)dataSegment;

        memcpy(codeData, dataSegment, finfo->size);

        let gdt = (SegmentDescriptor *) GDT_ADDR;
        setSegmentDescriptor(gdt + 1003, dataSegmentSize, (uint)dataSegment, AR_CODE32_ER + 0x60);
        setSegmentDescriptor(gdt + 1004, dataSegmentSize, (uint)dataSegment, AR_DATA32_RW + 0x60);

        let thisTask = taskController->currentTask();

        let appIOQueueBuffer = (uint *)memoryManager->allocate(1024);
        UIntQueue appIOQueue(appIOQueueBuffer, 1024, thisTask);
        consoleData->appIOQueue = &appIOQueue;

        startApp(elfHeader->entry, 1003 * 8, finfo->size + bssSize, 1004 * 8, &(thisTask->tss.esp0));

        memoryManager->release(appIOQueueBuffer, 1024);
        consoleData->appIOQueue = nullptr;

        layerController->releaseLayersAssociatedWithTask(thisTask);
        timerController->removeTimersAssociatedWithTask(thisTask);

        memset(_dataSegment, 0, (size_t)dataSegmentSize);

        memoryManager->releasePageAlign(codeData,     finfo->size);
        memoryManager->releasePageAlign(_dataSegment, dataSegmentSize + 16);

        _consoleLineFeed(consoleData);
    }
}

void switchFocusedLayer(Layer *layer);

extern "C" void *_consoleApi(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {
    let consoleData = *((ConsoleData **) 0x0fec);
    let thisTask = taskController->currentTask();
    int *reg = &eax + 1;

    if (edx == 1) { 
        _consolePutCharUtf32(consoleData, eax);
    } else if (edx == 2) {
        _consolePutString(consoleData, (char*) (appDSBase + ebx));
    } else if (edx == 5) {
        let layer = layerController->newLayer((byte*) (appDSBase + ebx), esi, edi + 24, eax, thisTask);
        drawWindow(layer->buffer, layer->width, layer->height, (const char *) (appDSBase + ecx), eax, false);
        layer->setPosition(100, 50);
        layer->setZIndex(layerController->getCount());

        switchFocusedLayer(layer);

        reg[7] = (int) layer;
    } else if (edx == 6) {
        let layer = (Layer *) ((uint)ebx & ~0x1u);
        let length = strlen((const char *) (appDSBase + ebp));

        paintString(layer->buffer, layer->width, esi, edi + 24, eax, (const char *) (appDSBase + ebp));

        if ((ebx & 1) == 0) {
            windowRoundCorner(layer->buffer, layer->width, layer->height, layer->transparentColor);
            layerController->refresh(layer->x + esi, layer->y + edi + 24, length * 8, 16);
        }
            
    } else if (edx == 7) {
        let layer = (Layer *) ((uint)ebx & ~0x1u);
        fillRect(layer->buffer, layer->width, ebp, eax, ecx + 24, esi, edi);

        if ((ebx & 1) == 0) {
            windowRoundCorner(layer->buffer, layer->width, layer->height, layer->transparentColor);
            layerController->refresh(layer->x + eax, layer->y + ecx + 24, esi, edi);
        }
            
    } else if (edx == 8) {
        forever {
            io_cli();

            if (!consoleData->appIOQueue->isEmpty()) {
                reg[7] = consoleData->appIOQueue->pop();
                io_sti();
                return nullptr;
            }

            if (!eax) {
                reg[7] = -1;
                io_sti();
                return nullptr;
            }

            io_stihlt();
        }
    } else if (edx == 11) {
        let layer = (Layer *) ((uint)ebx & ~0x1u);

        layer->buffer[(edi + 24) * layer->width + esi] = eax;

        if ((ebx & 1) == 0) {
            windowRoundCorner(layer->buffer, layer->width, layer->height, layer->transparentColor);
            layerController->refresh(layer->x + esi, layer->y + edi + 24, 1, 1);
        }
    } else if (edx == 12) {
        let layer = (Layer *) ebx;

        windowRoundCorner(layer->buffer, layer->width, layer->height, layer->transparentColor);
        layerController->refresh(layer->x + eax, layer->y + ecx + 24, esi, edi);
    } else if (edx == 13) {
        let layer = (Layer *) ((uint)ebx & ~0x1u);

        drawLine(layer->buffer, layer->width, eax, ecx + 24, esi, edi + 24, (byte)ebp);

        if ((ebx & 1) == 0) {
            windowRoundCorner(layer->buffer, layer->width, layer->height, layer->transparentColor);
            layerController->refresh(layer->x + eax, layer->y + ecx + 24, esi - eax, edi - ecx);
        }
    } else if (edx == 14) {
        let layer = (Layer *) ebx;

        layer->release();
    } else if (edx == 15) {
        forever {
            io_cli();

            while (!consoleData->ioQueue->isEmpty()) {
                let dataRaw = consoleData->ioQueue->pop();
                let type = (dataRaw & 0xff000000) >> 24;
                let data = dataRaw & 0x00ffffff;

                if (type == IOQUEUE_CONTROL_IN) {
                    if (data == 0) {
                        consoleData->cursorEnabled = false;
                    } else if (data == 1) {
                        consoleData->cursorEnabled = true;
                    }
                } else if (type == IOQUEUE_KEYBOARD_IN) {
                    io_sti();
                    reg[7] = data;
                    return nullptr;
                } else if (type == IOQUEUE_TIMER_IN) {
                    if (data == 1) {
                        consoleData->cursorTimer->reset(timerController->current() + 50, 1);
                    }
                }
            }

            if (!eax) {
                io_sti();
                reg[7] = -1;
                return nullptr;
            }

            io_stihlt();
        }
    } else if (edx == 16) {
        reg[7] = (int)timerController->newTimer(consoleData->appIOQueue, eax + timerController->current(), ecx, thisTask);
    } else if (edx == 17) {
        ((Timer *)ebx)->reset(eax + timerController->current(), ecx);
    } else if (edx == 19) {
        let timer = ((Timer *)ebx);

        timer->release();
    } else if (edx == 20) {
        if (eax == 0) {
            io_out8(0x61, io_in8(0x61) & 0x0d);
        } else {
            io_out8(0x43, 0xb6);
            
            let freq = 1193180000u / (uint)eax;

            io_out8(0x42, freq & 0xff);
            io_out8(0x42, freq >> 8);

            io_out8(0x61, (io_in8(0x61) | 0x03) & 0x0f);
        }
    } else if (edx == 65536) {
        return &(thisTask->tss.esp0);
    }

    return nullptr;
}

void consoleTask(Layer *layer) {
    Task *thisTask = taskController->currentTask();

    let bootInfo = (BootInfo *) BOOTINFOADDR;

    uint ioQueueBuffer[256];
    UIntQueue ioQueue(ioQueueBuffer, 256, thisTask);

    thisTask->queue = &ioQueue;

    ConsoleData consoleData;
    *((ConsoleData**) 0x0fec) = &consoleData;

    consoleData.fat = (int *) memoryManager->allocatePageAlign(4 * 2880);
    loadFat12To(consoleData.fat, (byte *) (DISKIMAGEADDR + 0x000200));

    consoleData.nextCursorX = 8;
    consoleData.nextCursorY = 32;

    consoleData.layer = layer;

    consoleData.ioQueue = &ioQueue;
    consoleData.cursorTimer = timerController->newTimer(&ioQueue, 50, 1);
    
    var cursorLit = true;

    var lShift = false, rShift = false;
    var putCount = 0;
    char inputBuffer[256];
    inputBuffer[0] = 0;

    _consolePutString(&consoleData, "admin@computer:/ $ ");

    forever {
        io_cli();

        while (!ioQueue.isEmpty()) {
            let dataRaw = ioQueue.pop();
            let type = (dataRaw & 0xff000000) >> 24;
            let data = dataRaw & 0x00ffffff;

            if (type == IOQUEUE_CONTROL_IN) {
                if (data == 0) {
                    consoleData.cursorEnabled = false;
                } else if (data == 1) {
                    consoleData.cursorEnabled = true;
                    cursorLit = true;
                    _consoleRefreshCursor(&consoleData, cursorLit && consoleData.cursorEnabled);
                }
            } else if (type == IOQUEUE_KEYBOARD_IN) {
                if (data == 0x1c) {
                    fillRect(layer->buffer, layer->width, HARIB_COL_000, consoleData.nextCursorX, consoleData.nextCursorY, 2, 16);
                    layerController->refresh(layer->x + consoleData.nextCursorX, layer->y + consoleData.nextCursorY, 2, 16);

                    inputBuffer[putCount] = 0;
                    _consoleLineFeed(&consoleData);

                    _consoleHandleCommand(&consoleData, inputBuffer);

                    _consolePutString(&consoleData, "admin@computer:/ $ ");
                    
                    inputBuffer[0] = 0;

                    _consoleRefreshCursor(&consoleData, cursorLit && consoleData.cursorEnabled);

                    putCount = 0;
                } else if (data == 0x2a) {
                    lShift = true;
                } else if (data == 0xaa) {
                    lShift = false;
                } else if (data == 0x36) {
                    rShift = true;
                } else if (data == 0xb6) {
                    rShift = false;
                } else if (data == 0x0e) {
                    if (putCount > 0) {
                        fillRect(layer->buffer, layer->width, HARIB_COL_000, consoleData.nextCursorX - 8, consoleData.nextCursorY, 10, 16);
                        layerController->refresh(layer->x + consoleData.nextCursorX - 8, layer->y + consoleData.nextCursorY, 10, 16);
                    
                        consoleData.nextCursorX -= 8;
                        inputBuffer[putCount-- - 1] = 0;

                        _consoleRefreshCursor(&consoleData, cursorLit && consoleData.cursorEnabled);
                    }
                } else if (data < 0x54 && keyCodeToLower[data] != 0) {
                    let caps = (bootInfo->leds & 4) != 0;
                    let charRaw = keyCodeToLower[data];
                    let shouldInv = caps && (charRaw >= 'A' && charRaw <= 'Z' || charRaw >= 'a' && charRaw <= 'z');
                    let character = (lShift ^ rShift ^ shouldInv ? keyCodeToUpper : keyCodeToLower)[data];

                    fillRect(layer->buffer, layer->width, HARIB_COL_000, consoleData.nextCursorX, consoleData.nextCursorY, 8, 16);
                    paintChar(
                        layer->buffer, layer->width, consoleData.nextCursorX, consoleData.nextCursorY, HARIB_COL_FFF, 
                        character
                    );
                    layerController->refresh(layer->x + consoleData.nextCursorX, layer->y + consoleData.nextCursorY, 8, 16);

                    consoleData.nextCursorX += 8;
                    inputBuffer[putCount++] = character;
                }

                cursorLit = true;

                _consoleRefreshCursor(&consoleData, cursorLit && consoleData.cursorEnabled);
            } else if (type == IOQUEUE_TIMER_IN) {
                if (data == 1) {
                    cursorLit = !cursorLit;
                    _consoleRefreshCursor(&consoleData, cursorLit && consoleData.cursorEnabled);

                    consoleData.cursorTimer->reset(timerController->current() + 50, 1);
                }
            }
        }
    
        io_sti();
    }
}
