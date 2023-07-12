#include "../api/cosplay.h"

char buffer[300 * (32 + 24)];

int main() {
    auto window = apiOpenWindow((unsigned char *)buffer, 300, 32, 99, "Hello, world!");

    apiFillRect(window, 8, 8 + 8, 300 - 16, 8, 3);
    apiPaintString(window, 8, 8, 0, "Hello! How are you?");

    return 0;
}