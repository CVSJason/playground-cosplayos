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

Timer *TimerController::newTimer(UIntQueue *queue, uint timeoutTime, uint dataToSend, Task *task) {
    for_until(i, 0, MAX_TIMER) {
        if (timers[i].flags == 0) {
            timers[i].timerController = this;
            timers[i].queue = queue;
            timers[i].task = task;
            timers[i].reset(timeoutTime, dataToSend);
            
            return timers + i;
        }
    }

    return nullptr;
}

void Timer::release() {
    if (flags == TIMER_FLAGS_USING) {
        cancel();
    }

    flags = 0;
}

void Timer::reset(uint timeoutTime, uint dataToSend) {
    if (flags == TIMER_FLAGS_USING) {
        cancel();
    }

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

void Timer::cancel() {
    let eflags = io_load_eflags();
    let timerController = ((TimerController *)this->timerController);

    io_cli();

    if (flags == TIMER_FLAGS_USING) {
        if (this == timerController->nextTimer) {
            timerController->nextTimer = timerController->nextTimer->nextTimer;
        } else {
            var curr = timerController->nextTimer;

            while (this != curr->nextTimer) {
                curr = curr->nextTimer;
            }

            curr->nextTimer = curr->nextTimer->nextTimer;
        }

        flags = TIMER_FLAGS_IDLE;
        io_store_eflags(eflags);
        return;
    }

    io_store_eflags(eflags);
    return;
}

void TimerController::removeTimersAssociatedWithTask(Task *task) {
    let eflags = io_load_eflags();

    io_cli();

    while (nextTimer->task == task) nextTimer = nextTimer->nextTimer;

    var currentTimer = nextTimer;

    while (currentTimer->nextTimer != nullptr) {
        while (currentTimer->nextTimer->task == task) {
            currentTimer->nextTimer = currentTimer->nextTimer->nextTimer;
        }

        currentTimer = currentTimer->nextTimer;
    }

    io_store_eflags(eflags);
}