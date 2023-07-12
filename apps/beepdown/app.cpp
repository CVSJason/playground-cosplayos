#include "../api/cosplay.h"
#include "stdio.h"

int main() {
    auto timer = apiCreateTimer(1, 1);

    for (int i = 20000000; i >= 20000; i -= i / 100) {
        static char str[10];

        sprintf(str, "%d\n", i);
        apiPutString(str);

        apiBeep(i);

        while (true) {
            unsigned int dataRaw = apiReadQueue();
            unsigned int type = (dataRaw & 0xff000000) >> 24;
            unsigned int data = dataRaw & 0xff;

            if (type == IOQUEUE_TIMER_IN && data == 1) {
                apiResetTimer(timer, 1, 1);

                break;
            }
        }
    }

    apiBeep(0);
}