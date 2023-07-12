#pragma once

extern "C" void apiPutCharUtf32(int chara);
extern "C" void apiPutString(const char *str);
extern "C" void *apiOpenWindow(unsigned char *buffer, int width, int height, int transparentColor, const char *title);
extern "C" void apiPaintString(void *handle, int x, int y, int color, const char *str, bool refresh = true);
extern "C" void apiFillRect(void *handle, int x, int y, int width, int height, int color, bool refresh = true);
extern "C" void apiSetPixel(void *handle, int x, int y, int color, bool refresh = true);
extern "C" void apiRefresh(void *handle, int x, int y, int width, int height);