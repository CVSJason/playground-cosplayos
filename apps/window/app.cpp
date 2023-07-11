#include "../api/cosplay.h"

char buffer[300 * 100];

int main() {
    auto window = apiOpenWindow((unsigned char *)buffer, 300, 56, 99, "Hello, world!");

    apiFillRect(window, 8, 44, 300 - 16, 4, 3);
    apiPaintString(window, 8, 32, 0, "Hello! How are you?");

    return 0;
}