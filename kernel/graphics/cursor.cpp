#include "../utils.h"
#include "graphics.h"

const char* cursor[16] = {
    "*...............",
    "**..............",
    "*o*.............",
    "*oo*............",
    "*ooo*...........",
    "*oooo*..........",
    "*ooooo*.........",
    "*oooooo*........",
    "*ooooooo*.......",
    "*oooooooo*......",
    "*ooooo****......",
    "*ooo*o*.........",
    "*oo**o*.........",
    "*o*..*o*........",
    "**...*o*........",
    ".....***........",
};

void writeCursorTexture(byte *texture, byte background) {
    for_until(y, 0, 16) {
        for_until(x, 0, 16) {
            if (cursor[y][x] == '*') {
                texture[y * 16 + x] = HARIB_COL_FFF;
            } else if (cursor[y][x] == 'o') {
                texture[y * 16 + x] = HARIB_COL_000;
            } else {
                texture[y * 16 + x] = background;
            }
        }
    }
}