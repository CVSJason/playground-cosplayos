#include "../utils.h"
#include "hardware.h"

void initPit() {
    io_out8(0x0043, 0x34);
    io_out8(0x0040, 0x9c);
    io_out8(0x0040, 0x2e);
}

void TimerController::proceed() {
    timer++;
    bool toTaskSwitch = false;

    while (timer > nextTimer->timeoutTime) {
        if (nextTimer->dataToSend == 0xffffff) toTaskSwitch = true;
        else nextTimer->queue->push(nextTimer->dataToSend, IOQUEUE_TIMER_IN);

        nextTimer->flags = TIMER_FLAGS_IDLE;

        nextTimer = nextTimer->nextTimer;
    }

    if (toTaskSwitch) {
        taskSwitch();
    }
}

Timer *TimerController::newTimer(UIntQueue *queue, uint timeoutTime, uint dataToSend) {
    for_until(i, 0, MAX_TIMER) {
        if (timers[i].flags == 0) {
            timers[i].timerController = this;
            timers[i].queue = queue;
            timers[i].reset(timeoutTime, dataToSend);
            
            return timers + i;
        }
    }

    return nullptr;
}

void Timer::release() {
    flags = 0;
}

void Timer::reset(uint timeoutTime, uint dataToSend) {
    this->timeoutTime = timeoutTime;
    this->dataToSend = dataToSend;
    this->flags = TIMER_FLAGS_USING;
    this->nextTimer = nullptr;

    let tc = (TimerController *)timerController;

    tc->addTimer(this);
}

void TimerController::addTimer(Timer *timer) {
    if (timer->timeoutTime <= nextTimer->timeoutTime) {
        timer->nextTimer = nextTimer;
        nextTimer = timer;

        return;
    }

    var current = nextTimer;

    while (timer->timeoutTime > current->nextTimer->timeoutTime) {
        current = current->nextTimer;
    }

    timer->nextTimer = current->nextTimer;
    current->nextTimer = timer;
}   