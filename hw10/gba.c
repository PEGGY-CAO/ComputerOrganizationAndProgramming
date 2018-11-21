#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
    // TA-TODO: IMPLEMENT

    // Write a while loop that loops until we're NOT in vBlank anymore:
    // (This prevents counting one VBlank more than once if your app is too fast)
	while(*SCANLINECOUNTER < 160);

    // Write a while loop that keeps going until we're in vBlank:
	while(*SCANLINECOUNTER > 160);

    // Finally, increment the vBlank counter:
	vBlankCounter++;

}

static int __qran_seed= 42;
static int qran(void) {
    __qran_seed= 1664525*__qran_seed+1013904223;
    return (__qran_seed>>16) & 0x7FFF;
}

int randint(int min, int max) {
    return (qran()*(max-min)>>15)+min;
}

void setPixel(int x, int y, u16 color) {
    // TA-TODO: IMPLEMENT
    videoBuffer[y * 240 + x] = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color) {
    // TA-TODO: IMPLEMENT
	for (int i = 0; i < height; i++) {
		DMA[3].src = &color;
    	DMA[3].dst = videoBuffer + OFFSET(i + x, y, 240);
    	DMA[3].cnt = DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED | DMA_ON | width;
    }
}

void drawFullScreenImageDMA(const u16 *image) {
    // TA-TODO: IMPLEMENT
    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT | DMA_ON | 240 * 160;
}

void drawImageDMA(int x, int y, int width, int height, const u16 *image) {
    // TA-TODO: IMPLEMENT
    for (int i = 0; i < height; i++) {
		DMA[3].src = image + OFFSET(i,0,width);
		DMA[3].dst = videoBuffer + OFFSET(i+x, y, 240);
		DMA[3].cnt = width | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT;
	}
}

void fillScreenDMA(volatile u16 color) {
    // TA-TODO: IMPLEMENT
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED | DMA_ON | 240 * 160;
}

void drawChar(int col, int row, char ch, u16 color) {
    for(int r = 0; r<8; r++) {
        for(int c=0; c<6; c++) {
            if(fontdata_6x8[OFFSET(r, c, 6) + ch*48]) {
                setPixel(col+c, row+r, color);
            }
        }
    }
}

void drawString(int col, int row, char *str, u16 color) {
    while(*str) {
        drawChar(col, row, *str++, color);
        col += 6;
    }
}

void drawCenteredString(int x, int y, int width, int height, char *str, u16 color) {
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy) {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}
