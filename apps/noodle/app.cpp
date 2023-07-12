#include "../api/cosplay.h"
#include "stdio.h"

unsigned char buffer[96 * (32 + 24)];

int main() {
    auto window = apiOpenWindow(buffer, 96, 32, 99, "Noodle!");

    static char str[10];

    int hours = 0, minutes = 0, seconds = 0;

    sprintf(str, "%02d:%02d:%02d", hours, minutes, seconds);
    apiFillRect(window, 16, 8, 64, 16, 16, false);
    apiPaintString(window, 16, 8, 0, str, false);
    apiRefresh(window, 16, 8, 64, 16);

    auto timer = apiCreateTimer(100, 1);

    while (true) {
        unsigned int dataRaw = apiReadQueue();
        unsigned int type = (dataRaw & 0xff000000) >> 24;
        unsigned int data = dataRaw & 0xff;

        if (type == IOQUEUE_TIMER_IN && data == 1) {
            seconds++;

            if (seconds == 60) {
                seconds = 0;
                minutes++;
            }
            if (minutes == 60) {
                minutes = 60;
                hours++;
            }
            if (hours == 100) {
                hours = 0;
            }

            sprintf(str, "%02d:%02d:%02d", hours, minutes, seconds);
            apiFillRect(window, 16, 8, 64, 16, 16, false);
            apiPaintString(window, 16, 8, 0, str, false);
            apiRefresh(window, 16, 8, 64, 16);

            apiResetTimer(timer, 100, 1);
        }
    }
}