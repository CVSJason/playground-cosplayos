#include "../utils.h"
#include "graphics.h"

unsigned char colorData[17 * 3] = {
	0x00, 0x00, 0x00,	/*  0:黑色 */
	0xff, 0x00, 0x00,	/*  1:淡红 */
	0x00, 0xff, 0x00,	/*  2:淡绿 */
	0xff, 0xff, 0x00,	/*  3:淡黄 */
	0x00, 0x00, 0xff,	/*  4:淡蓝 */
	0xff, 0x00, 0xff,	/*  5:淡紫 */
	0x00, 0xff, 0xff,	/*  6:淡青 */
	0xff, 0xff, 0xff,	/*  7:白色 */
	0xc6, 0xc6, 0xc6,	/*  8:淡灰 */
	0x84, 0x00, 0x00,	/*  9:暗红 */
	0x00, 0x84, 0x00,	/* 10:暗绿 */
	0x84, 0x84, 0x00,	/* 11:暗黄 */
	0x00, 0x00, 0x84,	/* 12:暗蓝 */
	0x84, 0x00, 0x84,	/* 13:暗紫 */
	0x00, 0x84, 0x84,	/* 14:暗青 */
	0x84, 0x84, 0x84	/* 15:暗灰 */,
	0xf0, 0xf0, 0xf0    /* 16:窗口背景 */
}; 

void setPalette(int start, int end, unsigned char *data) {
    int eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, start);

    for (int i = start; i < end; i++) {
        io_out8(0x03c9, data[0] >> 2);
        io_out8(0x03c9, data[1] >> 2);
        io_out8(0x03c9, data[2] >> 2);

		data += 3;
	}

    io_store_eflags(eflags);
}

void initPalette() {
    setPalette(0, 16, colorData);

	static byte otherColorData[216 * 3];

	for_until(b, 0, 6) {
		for_until(g, 0, 6) {
			for_until(r, 0, 6) {
				let baseIdx = (r + g * 6 + b * 36) * 3;

				otherColorData[baseIdx + 0] = r * 51;
				otherColorData[baseIdx + 1] = g * 51;
				otherColorData[baseIdx + 2] = b * 51;
			}
		}
	}

	setPalette(40, 256, otherColorData);
}