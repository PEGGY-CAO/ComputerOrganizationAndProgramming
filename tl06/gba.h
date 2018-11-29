#include "uint.h"

#define REG_DISPCNT *(volatile unsigned short *) 0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10)

#define COLOR(r, g, b) ((r) | (g)<<5 | (b)<<10)
#define WHITE COLOR(31,31,31)
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define MAGENTA COLOR(31, 0, 31)
#define CYAN COLOR(0, 31, 31)
#define YELLOW COLOR(31, 31, 0)
#define BLACK 0
#define GRAY COLOR(5, 5, 5)

extern volatile unsigned short *videoBuffer;
extern const unsigned char fontdata_6x8[12288];

#define BUTTON_A			(1<<0)
#define BUTTON_B			(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP			(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R			(1<<8)
#define BUTTON_L			(1<<9)

#define BUTTONS *(volatile u32 *)0x4000130
#define KEY_DOWN(key, buttons)  (~(buttons) & (key))
#define KEY_PRESSED(k, ob, nb) (!KEY_DOWN((k), (ob)) && KEY_DOWN((k), (nb)))

// Defines
#include "dma.h"

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)
#define SCANLINECOUNTER (volatile unsigned short *)0x4000006

extern u32 vBlankCounter;

void waitForVBlank(void);

#define TRUE  1
#define FALSE 0

#define UNUSED(param) ((void)((param)))

void setPixel(int x, int y, u16 color);
void drawRectDMA(int x, int y, int width, int height, volatile u16 color);
void drawChar(int col, int row, char ch, u16 color);
void drawString(int col, int row, char *str, u16 color);
