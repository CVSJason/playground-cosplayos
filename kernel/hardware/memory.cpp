#include "../utils.h"
#include "hardware.h"

volatile uint memoryTest(uint start, uint end) {
    bool is486 = false;

    uint eflag = io_load_eflags();
    eflag |= 0x00040000;
    io_store_eflags(eflag);

    eflag = io_load_eflags();
    if ((eflag & 0x00040000) != 0) {
        is486 = true;
    }

    eflag = io_load_eflags();
    eflag &= ~0x00040000;
    io_store_eflags(eflag);

    if (is486) store_cr0(load_cr0() | 0x60000000);
    
    var result = start;

    for (; result <= end; result += 0x100000) {
        let target = (uint *)(result + 0xffffc);
        let old = *target;
        var valid = true;

        *target = 0xaa55aa55;
        *target ^= 0xffffffff;

        if (*target != 0x55aa55aa) valid = false;

        *target ^= 0xffffffff;

        if (*target != 0xaa55aa55) valid = false;

        *target = old;

        if (!valid) break;
    }

    if (is486) store_cr0(load_cr0() & ~0x60000000);

    return result;
}

uint MemoryManager::calculateFree() {
    uint size = 0;
    
    for_until(i, 0, entryCount) {
        size += entries[i].size;
    }

    return size;
}

void *MemoryManager::allocate(uint size) {
    for_until(i, 0, entryCount) {
        if (entries[i].size >= size) {
            let result = entries[i].address;

            entries[i].address = (void*)((uint)(entries[i].address) + size);
            entries[i].size -= size;

            if (entries[i].size == 0) {
                entryCount--;

                for_until(j, i, entryCount) {
                    entries[j] = entries[j + 1];
                }
            }

            return result;
        }
    }

    return nullptr;
}

int MemoryManager::release(void *addr, uint size) {
    var insertPoint = 0;

    for (; insertPoint < entryCount; insertPoint++) {
        if (entries[insertPoint].address > addr) break;
    }

    var succeed = false;

    if (insertPoint > 0 && (uint)entries[insertPoint - 1].address + entries[insertPoint - 1].size == (uint)addr) {
        succeed = true;
        
        entries[insertPoint - 1].size += size;
    }

    if (insertPoint < entryCount && (uint)addr + size == (uint)entries[insertPoint].address) {
        if (succeed) {
            entries[insertPoint - 1].size += entries[insertPoint].size;
            entryCount--;

            for_until(j, insertPoint, entryCount) {
                entries[j] = entries[j + 1];
            }
        } else {
            entries[insertPoint].address = addr;
            entries[insertPoint].size += size;

            succeed = true;
        }
    }

    if (succeed) return 0;

    if (entryCount < 4090) {
        for (var j = entryCount; j > insertPoint; j--) {
            entries[j] = entries[j - 1];
        }

        entryCount++;

        if (entryCountMax < entryCount) entryCountMax = entryCount;

        entries[insertPoint].address = addr;
        entries[insertPoint].size    = size;

        return 0;
    }

    lostCount++;
    lostSize += size;

    return -1;
}