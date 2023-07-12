#include "../api/cosplay.h"

char buffer[96 * (48 + 24)];

int main() {
    auto window = apiOpenWindow((unsigned char *)buffer, 96, 48, 99, "lines");

    for (int i = 0; i <= 8; i++) {
        apiDrawLine(window, 8, 8, 40, 8 + i * 4, i, false);
        apiDrawLine(window, 56, 8, 56 + i * 4, 40, i, false);
    }

    apiRefresh(window, 0, 0, 96, 48);

    while (apiReadKey(true) != 0x1c);

    apiCloseWindow(window);

    return 0;
}