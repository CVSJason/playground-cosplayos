#pragma once

extern "C" void apiPutCharUtf32(int chara);
extern "C" void apiPutString(const char *str);
extern "C" void *apiOpenWindow(unsigned char *buffer, int width, int height, int transparentColor, const char *title);
extern "C" void apiPaintString(void *handle, int x, int y, int color, const char *str, bool refresh = true);
extern "C" void apiFillRect(void *handle, int x, int y, int width, int height, int color, bool refresh = true);
extern "C" void apiSetPixel(void *handle, int x, int y, int color, bool refresh = true);
extern "C" void apiRefresh(void *handle, int x, int y, int width, int height);
extern "C" void apiDrawLine(void *handle, int x0, int y0, int x1, int y1, int color, bool refresh = true);
extern "C" int apiReadKey(bool toSleep = true);
extern "C" int apiCloseWindow(void *handle);
extern "C" unsigned int apiReadQueue(bool toSleep = true);
extern "C" void *apiCreateTimer(unsigned int timeOut, unsigned int dataToSend);
extern "C" void apiResetTimer(void *handle, unsigned int timeOut, unsigned int dataToSend);
extern "C" void apiReleaseTimer(void *handle);
extern "C" void apiBeep(int frequency);

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

#define IOQUEUE_KEYBOARD_IN  0x01
#define IOQUEUE_MOUSE_IN     0x02
#define IOQUEUE_TIMER_IN     0x03
#define IOQUEUE_CONTROL_IN   0x04
#define IOQUEUE_KEYBOARD_OUT 0x81