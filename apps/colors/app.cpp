#include "../api/cosplay.h"

unsigned char buffer[130 * (129 + 24)];

unsigned char dither(int r, int g, int b, int x, int y) {
    static int table[] = { 3, 1, 0, 2 };

    x &= 1;
    y &= 1;

    int i = table[x + y * 2];

    r = ((r * 21) / 256 + i) / 4;
    g = ((g * 21) / 256 + i) / 4;
    b = ((b * 21) / 256 + i) / 4;

    return 40 + r + g * 6 + b * 36;
}

int main() {
    auto window = apiOpenWindow(buffer, 130, 129, 39, "Colors!");

    for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 128; x++) {
            buffer[(x + 1) + (y + 24) * 130] = dither(x * 2, y * 2, 0, x, y);
        }
    }

    apiRefresh(window, 1, 0, 128, 128);

    for (;;);
}