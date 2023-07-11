#pragma once

struct ConsoleData {
    Layer *layer;
    int nextCursorX, nextCursorY;
    int *fat;
};

void consoleTask(Layer *layer);