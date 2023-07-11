#include "../utils.h"
#include "stdio.h"
#include "string.h"
#include "graphics.h"

void fillRect(byte *vram, int screenWidth, byte color, int x, int y, int width, int height) {
    let xEnd = x + width, yEnd = y + height;

    for_until(currY, y, yEnd) {
        for_until(currX, x, xEnd) {
            vram[currY * screenWidth + currX] = color;
        }
    }
}

extern byte asciiFont[];

void paintChar(byte *vram, int screenWidth, int x, int y, byte color, char chara) {
    for_until(i, 0, 16) {
        let d = asciiFont[i + chara * 16];
        let loadBase = vram + ((y + i) * screenWidth + x);

        if ((d & 0x80) != 0) loadBase[0] = color;
        if ((d & 0x40) != 0) loadBase[1] = color;
        if ((d & 0x20) != 0) loadBase[2] = color;
        if ((d & 0x10) != 0) loadBase[3] = color;
        if ((d & 0x08) != 0) loadBase[4] = color;
        if ((d & 0x04) != 0) loadBase[5] = color;
        if ((d & 0x02) != 0) loadBase[6] = color;
        if ((d & 0x01) != 0) loadBase[7] = color;
    }
}

void paintString(byte *vram, int screenWidth, int x, int y, byte color, const char *str) {
    while (*str != 0) {
        paintChar(vram, screenWidth, x, y, color, *str);

        x += 8;
        str++;
    }
}

void paintStringToLayerAndRefresh(Layer *layer, int x, int y, byte fg, byte bg, const char *str) {
    let startX = x;

    while (*str != 0) {
        fillRect(layer->buffer, layer->width, bg, x, y, 8, 16);
        paintChar(layer->buffer, layer->width, x, y, fg, *str);

        x += 8;
        str++;
    }

    ((LayerController *)layer->layerController)->refresh(layer->x + startX, layer->y + y, x - startX, 16);
}

void blit(byte *vram, int screenWidth, byte *texture, int x, int y, int width, int height) {
    for_until(currY, 0, height) {
        for_until(currX, 0, width) {
            vram[(currY + y) * screenWidth + (currX + x)] = texture[currY * width + currX];
        }
    }
}

void drawBackground(byte *vram, int screenWidth, int screenHeight) {
    // 背景
    fillRect(vram, screenWidth, HARIB_COL_088, 0, 0, screenWidth, screenHeight);

    // 任务栏
    fillRect(vram, screenWidth, HARIB_COL_CCC, 0, screenHeight - 32, screenWidth, 32);
}

void drawWindowCaption(byte *vram, int width, int height, const char *name, byte transparentColor, bool isActive) {
    fillRect(vram, width, HARIB_COL_FFF, 1, 1, width - 2, 23);

    let textWidth = strlen(name) * 8;
    let textCol = isActive ? HARIB_COL_000 : HARIB_COL_888;

    if (textWidth > width - 8) {
        paintString(vram, width, width / 2 - 12, 4, textCol, "...");
        paintString(vram, width, width / 2 - 12 + 1, 4, textCol, "...");
    } else {
        paintString(vram, width, width / 2 - textWidth / 2, 4, textCol, name);
        paintString(vram, width, width / 2 - textWidth / 2 + 1, 4, textCol, name);
    }

    // 关闭键
    for_to(i, 0, 6) {
        vram[(9 + i) * width + width - 16 + i] = HARIB_COL_888;
        vram[(15 - i) * width + width - 16 + i] = HARIB_COL_888;
    }

    windowRoundCorner(vram, width, height, transparentColor);
} 

void drawWindow(byte *vram, int width, int height, const char *name, byte transparentColor, bool isActive) {
    // 背景
    fillRect(vram, width, 16, 0, 0, width, height);
    
    // 边框
    fillRect(vram, width, HARIB_COL_888, 0, 0, width, 1);
    fillRect(vram, width, HARIB_COL_888, 0, height - 1, width, 1);
    fillRect(vram, width, HARIB_COL_888, 0, 0, 1, height);
    fillRect(vram, width, HARIB_COL_888, width - 1, 0, 1, height);

    drawWindowCaption(vram, width, height, name, transparentColor, isActive);
}

void windowRoundCorner(byte *vram, int width, int height, byte transparentColor) {
    let right = width - 1, bottom = height - 1;

    vram[0 * width + 0] = transparentColor;
    vram[0 * width + 1] = transparentColor;
    vram[0 * width + 2] = transparentColor;
    vram[0 * width + 3] = transparentColor;
    vram[1 * width + 0] = transparentColor;
    vram[1 * width + 1] = transparentColor;
    vram[1 * width + 2] = HARIB_COL_888;
    vram[1 * width + 3] = HARIB_COL_888;
    vram[2 * width + 0] = transparentColor;
    vram[2 * width + 1] = HARIB_COL_888;
    vram[3 * width + 0] = transparentColor;
    vram[3 * width + 1] = HARIB_COL_888;

    vram[0 * width + right - 0] = transparentColor;
    vram[0 * width + right - 1] = transparentColor;
    vram[0 * width + right - 2] = transparentColor;
    vram[0 * width + right - 3] = transparentColor;
    vram[1 * width + right - 0] = transparentColor;
    vram[1 * width + right - 1] = transparentColor;
    vram[1 * width + right - 2] = HARIB_COL_888;
    vram[1 * width + right - 3] = HARIB_COL_888;
    vram[2 * width + right - 0] = transparentColor;
    vram[2 * width + right - 1] = HARIB_COL_888;
    vram[3 * width + right - 0] = transparentColor;
    vram[3 * width + right - 1] = HARIB_COL_888;

    vram[(bottom - 0) * width + 0] = transparentColor;
    vram[(bottom - 0) * width + 1] = transparentColor;
    vram[(bottom - 0) * width + 2] = transparentColor;
    vram[(bottom - 0) * width + 3] = transparentColor;
    vram[(bottom - 1) * width + 0] = transparentColor;
    vram[(bottom - 1) * width + 1] = transparentColor;
    vram[(bottom - 1) * width + 2] = HARIB_COL_888;
    vram[(bottom - 1) * width + 3] = HARIB_COL_888;
    vram[(bottom - 2) * width + 0] = transparentColor;
    vram[(bottom - 2) * width + 1] = HARIB_COL_888;
    vram[(bottom - 3) * width + 0] = transparentColor;
    vram[(bottom - 3) * width + 1] = HARIB_COL_888;

    vram[(bottom - 0) * width + right - 0] = transparentColor;
    vram[(bottom - 0) * width + right - 1] = transparentColor;
    vram[(bottom - 0) * width + right - 2] = transparentColor;
    vram[(bottom - 0) * width + right - 3] = transparentColor;
    vram[(bottom - 1) * width + right - 0] = transparentColor;
    vram[(bottom - 1) * width + right - 1] = transparentColor;
    vram[(bottom - 1) * width + right - 2] = HARIB_COL_888;
    vram[(bottom - 1) * width + right - 3] = HARIB_COL_888;
    vram[(bottom - 2) * width + right - 0] = transparentColor;
    vram[(bottom - 2) * width + right - 1] = HARIB_COL_888;
    vram[(bottom - 3) * width + right - 0] = transparentColor;
    vram[(bottom - 3) * width + right - 1] = HARIB_COL_888;
}