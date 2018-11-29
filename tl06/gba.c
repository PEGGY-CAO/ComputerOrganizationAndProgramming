#include "gba.h"

#include <stdio.h>

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;

u32 vBlankCounter = 0;

extern int  speed;
extern char frame_str[25];

void waitForVBlank(void)
{
    while (*SCANLINECOUNTER >= 160);
    while (*SCANLINECOUNTER <  160);

    vBlankCounter++;
}

void setPixel(int x, int y, u16 color)
{
    videoBuffer[OFFSET(y, x, WIDTH)] = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color)
{
    for (int row = 0; row < height; row++)
    {
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + OFFSET(row + y, x, WIDTH);
        DMA[3].cnt = DMA_ON | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | width;
    }
}

void drawChar(int col, int row, char ch, u16 color)
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 6; c++)
        {
            if (fontdata_6x8[OFFSET(r, c, 6) + ch * 48])
            {
                setPixel(col + c, row + r, color);
            }
        }
    }
}

void drawString(int col, int row, char *str, u16 color)
{
    while (*str)
    {
        col += 6;
        drawChar(col, row, *str++, color);
    }
}
