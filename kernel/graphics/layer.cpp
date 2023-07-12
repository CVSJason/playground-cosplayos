#include "../utils.h"
#include "graphics.h"

void LayerController::init(MemoryManager *manager, byte *vram, int width, int height) {
    this->layerMap = (byte *)manager->allocatePageAlign(width * height);
    this->vram = vram;
    this->screenWidth = width;
    this->screenHeight = height;
    this->topLayer = -1;

    for_until (i, 0, MAX_LAYER) {
        layerBuffer[i].flags = 0;
    }
}

Layer *LayerController::newLayer(byte *buffer, int width, int height, int transparentColor, Task *task) {
    for_until(i, 0, MAX_LAYER) {
        if (layerBuffer[i].flags == 0) {
            let layer = layerBuffer + i;

            layer->flags = LAYER_USING;
            layer->zIndex = -1;
            layer->buffer = buffer;
            layer->width = width;
            layer->height = height;
            layer->task = task;
            layer->transparentColor = transparentColor;
            layer->layerController = this;

            return layer;
        }
    }

    return nullptr;
}

void Layer::setZIndex(int zIndex) {
    let old = this->zIndex;
    let lc = ((LayerController*)layerController);

    if (zIndex > lc->topLayer + 1) {
        zIndex = lc->topLayer + 1;
    }
    if (zIndex < -1) {
        zIndex = -1;
    }
    this->zIndex = zIndex;

    lc->reorder(this, old);
} 

void LayerController::reorder(Layer *targetLayer, int oldZIndex) {
    let currentZIndex = targetLayer->zIndex;

    if (oldZIndex > currentZIndex) {
        if (currentZIndex >= 0) {
            for (var i = oldZIndex; i > currentZIndex; i--) {
                layers[i] = layers[i - 1];
                layers[i]->zIndex = i;
            }

            layers[currentZIndex] = targetLayer;
            refresh(targetLayer->x, targetLayer->y, targetLayer->width, targetLayer->height, true);
        } else {
            if (topLayer > oldZIndex) {
                for (var i = oldZIndex; i < topLayer; i++) {
                    layers[i] = layers[i + 1];
                    layers[i]->zIndex = i;
                }
            }

            topLayer--;
        }
    } else if (oldZIndex < currentZIndex) {
        if (oldZIndex >= 0) {
            for (var i = oldZIndex; i < currentZIndex; i++) {
                layers[i] = layers[i + 1];
                layers[i]->zIndex = i;
            }

            layers[currentZIndex] = targetLayer;
        } else {
            for (var i = topLayer; i >= currentZIndex; i--) {
                layers[i + 1] = layers[i];
                layers[i + 1]->zIndex = i + 1;
            }

            layers[currentZIndex] = targetLayer;
            topLayer++;
        }
    } else return;

    refreshMap(0, 0, screenWidth, screenHeight);
    refresh(targetLayer->x, targetLayer->y, targetLayer->width, targetLayer->height);
}

void LayerController::refreshMap(int clipX, int clipY, int clipWidth, int clipHeight, int toLayer) {
    if (toLayer < 0) toLayer = topLayer;
    if (toLayer > topLayer) toLayer = topLayer;

    clipX = max(clipX, 0);
    clipY = max(clipY, 0);
    clipWidth = min( screenWidth, clipX + clipWidth) - clipX;
    clipHeight = min(screenHeight, clipY + clipHeight) - clipY;

    if (clipWidth <= 0 || clipHeight <= 0) return;
    
    let clipXEnd = clipX + clipWidth;
    let clipYEnd = clipY + clipHeight;

    for_to(z, 0, toLayer) {
        let layer = layers[z];

        let refreshXStart = max(clipX, layer->x) - layer->x,
            refreshXEnd   = min(clipXEnd, layer->x + layer->width) - layer->x,
            refreshYStart = max(clipY, layer->y) - layer->y,
            refreshYEnd   = min(clipYEnd, layer->y + layer->height) - layer->y;

        var screenPos = (refreshYStart + layer->y) * screenWidth + layer->x + refreshXStart;
        var layerPos = refreshYStart * layer->width + refreshXStart;

        for_until(layerY, refreshYStart, refreshYEnd) {
            for_until(layerX, refreshXStart, refreshXEnd) {
                let color = layer->buffer[layerPos];

                if (color != layer->transparentColor) {
                    layerMap[screenPos] = z;
                }

                screenPos++;
                layerPos++;
            }

            screenPos += screenWidth - (refreshXEnd - refreshXStart);
            layerPos += layer->width - (refreshXEnd - refreshXStart);
        }
    }
}

void LayerController::refresh(int clipX, int clipY, int clipWidth, int clipHeight, bool moved) {
    clipX = max(clipX, 0);
    clipY = max(clipY, 0);
    clipWidth = min(screenWidth, clipX + clipWidth) - clipX;
    clipHeight = min(screenHeight, clipY + clipHeight) - clipY;

    if (clipWidth <= 0 || clipHeight <= 0) return;
    
    let clipXEnd = clipX + clipWidth;
    let clipYEnd = clipY + clipHeight;

    if (moved) refreshMap(clipX, clipY, clipWidth, clipHeight);

    for_to(z, 0, topLayer) {
        let layer = layers[z];

        var screenIdx = clipY * screenWidth + clipX;
        var layerIdx = (clipY - layer->y) * layer->width + (clipX - layer->x);

        for_until(screenY, clipY, clipYEnd) {
            for_until(screenX, clipX, clipXEnd) {
                if (layerMap[screenIdx] == z)
                    vram[screenIdx] = layer->buffer[layerIdx];

                screenIdx++;
                layerIdx++;
            }

            screenIdx += screenWidth - clipWidth;
            layerIdx += layer->width - clipWidth;
        }
    }    
}

void Layer::setPosition(int x, int y) {
    let oldX = this->x, oldY = this->y;

    this->x = x;
    this->y = y;

    if (zIndex >= 0) {
        ((LayerController*)layerController)->refresh(oldX, oldY, width, height, true);
        ((LayerController*)layerController)->refresh(x, y, width, height, true);
    }
    return;
}

void Layer::release() {
    if (zIndex >= 0) setZIndex(-1);

    flags = 0;
}

void LayerController::releaseLayersAssociatedWithTask(Task *task) {
    for_until(i, 0, MAX_LAYER) {
        if (layerBuffer[i].flags != 0 && layerBuffer[i].task == task) {
            layerBuffer[i].release();
        }
    }
}