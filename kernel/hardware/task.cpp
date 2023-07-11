#include "../utils.h"
#include "hardware.h"

Timer *multiTaskTimer;

extern TaskController *taskController;
extern MemoryManager *memoryManager;

void idleTask() {
    forever io_hlt();
}

void taskSwitch() {
    var level = taskController->taskLevels + taskController->currentLevel;
    let currentTask = level->tasks[level->current];

    level->current++;
    if (level->current == level->count) {
        level->current = 0;
    }

    if (taskController->toChangeLevel) {
        taskController->switchLevel();
        level = taskController->taskLevels + taskController->currentLevel;
    }

    let newTask = level->tasks[level->current];

    multiTaskTimer->reset(
        ((TimerController *)multiTaskTimer->timerController)->current() + level->tasks[level->current]->priority,
        0xffffff
    );

    if (newTask != currentTask) {
        jumpFar(0, level->tasks[level->current]->gdtId);
    }
}

Task *TaskController::init(TimerController *controller) {
    let gdt = (SegmentDescriptor *) GDT_ADDR;

    for_until(i, 0, MAX_TASKS) {
        taskBuffer[i].flags = 0;
        taskBuffer[i].gdtId = (TASK_GDT_START + i) * 8;
        setSegmentDescriptor(gdt + TASK_GDT_START + i, 103, (int) &taskBuffer[i].tss, AR_TSS32);
    }


    for_until(i, 0, MAX_TASK_LEVELS) {
        taskLevels[i].count = 0;
        taskLevels[i].current = 0;
    }

    let task = newTask();
    task->flags = 1;
    task->level = 0;

    addTaskToLevel(task);

    let idleTask = newTask();
    idleTask->tss.esp = (int) memoryManager->allocatePageAlign(64 * 1024) + 64 * 1024;
    idleTask->tss.eip = (int) ::idleTask;
    idleTask->tss.es = 1 * 8;
    idleTask->tss.cs = 2 * 8;
    idleTask->tss.ss = 1 * 8;
    idleTask->tss.ds = 1 * 8;
    idleTask->tss.fs = 1 * 8;
    idleTask->tss.gs = 1 * 8;
    run(idleTask, 0, MAX_TASK_LEVELS - 1);

    toChangeLevel = false;
    switchLevel();

    load_tr(task->gdtId);
    multiTaskTimer = controller->newTimer(nullptr, controller->current() + 3, 0xffffff);

    return task;
}

Task *TaskController::newTask() {
    for_until(i, 0, MAX_TASKS) {
        if (taskBuffer[i].flags == 0) {
            let task = taskBuffer + i;

            task->flags = 1;
            task->priority = 2;

            task->tss.eflags = 0x00000202;
            task->tss.eax = 0;
            task->tss.ecx = 0;
            task->tss.edx = 0;
            task->tss.ebx = 0;
            task->tss.ebp = 0;
            task->tss.esi = 0;
            task->tss.edi = 0;
            task->tss.es = 0;
            task->tss.ds = 0;
            task->tss.fs = 0;
            task->tss.gs = 0;
            task->tss.ldtr = 0;
            task->tss.iomap = 0x40000000;
            task->tss.ss0 = 0;

            task->taskController = this;

            return task;
        }
    }

    return nullptr;
}

void TaskController::run(Task *task, int priority, int level) {
    if (priority > 0) {
        task->priority = priority;
    }

    if (level < 0) {
        level = task->level;
    }

    if (task->flags == 2 && task->level != currentLevel) removeTaskFromLevel(task);

    if (task->flags != 2) {
        task->level = level;
        addTaskToLevel(task);
    }

    toChangeLevel = 1;
    return;
}

void TaskController::sleep(Task *task) {
    if (task->flags == 2) {
        let currTask = currentTask();
        removeTaskFromLevel(task);

        if (task == currTask) {
            switchLevel();
            let currTask = currentTask();
            jumpFar(0, currTask->gdtId);
        }
    }
}

Task *TaskController::currentTask() {
    let level = taskLevels + currentLevel;

    return level->tasks[level->current];
}

void TaskController::addTaskToLevel(Task *task) {
    let level = taskLevels + task->level;
    level->tasks[level->count++] = task;
    task->flags = 2;
}

void TaskController::removeTaskFromLevel(Task *task) {
    let level = taskLevels + task->level;

    int i = 0;
    for (; i < level->count; i++) {
        if (level->tasks[i] == task) {
            break;
        }
    }

    level->count--;

    if (i < level->current) level->current--;

    if (level->current >= level->count) level->current = 0;

    task->flags = 1;

    for (; i < level->count; i++) {
        level->tasks[i] = level->tasks[i + 1];
    }
}

void TaskController::switchLevel() {
    for_until(i, 0, MAX_TASK_LEVELS) {
        if (taskLevels[i].count > 0) {
            currentLevel = i;
            toChangeLevel = false;
            return;
        }
    }
}