#include "../utils.h"
#include "fs.h"

void loadFat12To(int *fat, byte *source) {
    var src = 0, dst = 0;

    for (; dst < 2880; dst += 2) {
        fat[dst + 0] = (source[src + 0]      | source[src + 1] << 8) & 0xfff;
        fat[dst + 1] = (source[src + 1] >> 4 | source[src + 2] << 4) & 0xfff;
        src += 3;
    }
}

void loadFileFat12(int firstCluster, int size, byte *buffer, int *fat, byte *fileDataStart) {
    var currentCluster = firstCluster;

    forever {
        if (size <= 512) {
            for_until(i, 0, size) {
                buffer[i] = fileDataStart[currentCluster * 512 + i];
            }

            break;
        }

        for_until(i, 0, 512) {
            buffer[i] = fileDataStart[currentCluster * 512 + i];
        }

        size -= 512;
        buffer += 512;
        currentCluster = fat[currentCluster];
    }
}

Fat12FileInfo *getFileInfo(char *fileName, Fat12FileInfo *startFileInfo) {
    var ext = fileName;

    for (var n = fileName; *n != 0; n++) {
        if (*n >= 'a' && *n <= 'z') *n = *n - 0x20;
    }

    while (*ext != '.' && *ext != 0) ext++;

    if (*ext == '.') {
        *ext = 0;
        ext++;
    }

    for_until(i, 0, 224) {
        if (startFileInfo[i].name[0] == 0) break;

        if ((startFileInfo[i].type & 0x18) == 0) {
            bool match = true;

            for_until(n, 0, 8) {
                if (fileName[n] != startFileInfo[i].name[n]) {
                    match = false; break;
                }
                
                if (fileName[n] == 0) break;
            }

            for_until(n, 0, 3) {
                if (ext[n] != startFileInfo[i].extension[n]) {
                    match = false; break;
                }
                
                if (ext[n] == 0) break;
            }

            if (!match) continue;

            return startFileInfo + i;
        }
    }

    return nullptr;
}