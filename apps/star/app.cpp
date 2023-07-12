#include "../api/cosplay.h"

char buffer[140 * (70 + 24)];

unsigned int rand_seed = 1;

int rand(void)
{
	rand_seed = rand_seed * 1103515245 + 12345;
	return (unsigned int) (rand_seed >> 16) & 32767;
}

int main() {
    auto window = apiOpenWindow((unsigned char *)buffer, 140, 70, 99, "The stars");

    apiFillRect(window, 1, 1, 140 - 2, 70 - 2, 0, false);

    for (int i = 0; i < 50; i++) {
        int x = (rand() % 137) + 1, y = (rand() % 67) + 1;

        apiSetPixel(window, x, y, 3, false);
    }

    apiRefresh(window, 0, 0, 140, 70);

    return 0;
}