#pragma once

#include "../hardware/hardware.h"

struct Layer {
    byte *buffer;
    int x, y, width, height, zIndex, flags, transparentColor;  
    void *layerController;

    void setZIndex(int zIndex);
    void setPosition(int x, int y);
    void release();
};

#define MAX_LAYER 256
#define LAYER_USING 1

class LayerController {
    friend Layer;

public:
    LayerController(const LayerController&) = delete;

    void init(MemoryManager *manager, byte *vram, int width, int height);
    Layer *newLayer(byte *buffer, int width, int height, byte transparentColor);
    void refresh(int clipX, int clipY, int clipWidth, int clipHeight, bool moved = false);

private:
    void reorder(Layer *targetLayer, int oldZIndex);

    byte *vram, *layerMap;
    int screenWidth, screenHeight, topLayer;
    Layer *layers[MAX_LAYER];
    Layer layerBuffer[MAX_LAYER];
};

/* palette.cpp */

#define HARIB_COL_000 0
#define HARIB_COL_F00 1
#define HARIB_COL_0F0 2
#define HARIB_COL_FF0 3
#define HARIB_COL_00F 4
#define HARIB_COL_F0F 5
#define HARIB_COL_0FF 6
#define HARIB_COL_FFF 7
#define HARIB_COL_CCC 8
#define HARIB_COL_800 9
#define HARIB_COL_080 10
#define HARIB_COL_880 11
#define HARIB_COL_008 12
#define HARIB_COL_808 13
#define HARIB_COL_088 14
#define HARIB_COL_888 15

void initPalette();

/* draw.cpp */
void fillRect(byte *vram, int screenWidth, byte color, int x, int y, int width, int height);
void paintChar(byte *vram, int screenWidth, int x, int y, byte color, char chara);
void paintString(byte *vram, int screenWidth, int x, int y, byte color, const char *str);
void paintStringToLayerAndRefresh(Layer *layer, int x, int y, byte fg, byte bg, const char *str);

void blit(byte *vram, int screenWidth, byte *texture, int x, int y, int width, int height);
void drawBackground(byte *vram, int screenWidth, int screenHeight);
void drawWindow(byte *vram, int width, int height, const char *name, byte transparentColor, bool isActive);
void drawWindowCaption(byte *vram, int width, int height, const char *name, byte transparentColor, bool isActive);
void windowRoundCorner(byte *vram, int width, int height, byte transparentColor);

/* cursor.cpp */
void writeCursorTexture(byte *texture, byte background);