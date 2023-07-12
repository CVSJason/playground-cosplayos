#pragma once

struct ConsoleData {
    Layer *layer;
    int nextCursorX, nextCursorY;
    int *fat;
    Timer *cursorTimer;
    UIntQueue *ioQueue;
    bool cursorEnabled;
};

void consoleTask(Layer *layer);