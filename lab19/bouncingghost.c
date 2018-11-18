#include "lib.h"
#include "ghost.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;

int main(void) {

	REG_DISPCNT = MODE3 | BG2_ENABLE;

	int row = 0;
	int col = 0;

	//loop infinitely
	while(1) {

		int up = 0;
		int down = 0;
		int left = 0;
		int right = 0;
		if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
       		col++; 
			right = 1;
    	}
    	if (KEY_DOWN_NOW(BUTTON_LEFT)) {
       		col--;
			left = 1;
    	}
    	if (KEY_DOWN_NOW(BUTTON_UP)) {
        	row--;
			up = 1;
    	}
    	if (KEY_DOWN_NOW(BUTTON_DOWN)) {
        	row++; 
			down = 1;
    	}
		//TODO: put something here to wait for a screen refresh before continuing
		waitForVblank();

		drawGhost(row, col, GHOST_WIDTH, GHOST_HEIGHT, ghost);

		//When ghost moves, it leaves a residue behind! There are four scenarios of movement to clean up behind the ghost

		//TODO: Scenario 1: ghost moves up (ie row--)
		if (up == 1) {
			int oriR = row + 1;
       		drawRectangle(oriR, col, GHOST_WIDTH, GHOST_HEIGHT, BLACK);
    	}
		
		//TODO: Scenario 2: ghost moves down (ie row++)
		if (down == 1) {
			int oriR = row - 1;
       		drawRectangle(oriR, col, GHOST_WIDTH, GHOST_HEIGHT, BLACK);
    	}

		//TODO: Scenario 3: ghost moves left (ie col--)
		if (left == 1) {
			int oriC = col + 1;
       		drawRectangle(row, oriC, GHOST_WIDTH, GHOST_HEIGHT, BLACK);
    	}

		//TODO: Scenario 4: ghost moves right (ie col++)
		if (right == 1) {
			int oriC = col - 1;
       		drawRectangle(row, oriC, GHOST_WIDTH, GHOST_HEIGHT, BLACK);
    	}

	}

}


//DO NOT EDIT ANY OF THESE FUNCTIONS
void setPixel(int row, int col, u16 color)
{ 
	videoBuffer[row*WIDTH + col] = color;
}

void drawRectangle(int row, int col, int width, int height, u16 color)
{
	for (int x = 0; x < height; x++) {
    	for (int y = 0; y < width; y++) {
	    	setPixel(row + x, col + y, color);	
		}
    }
}

void waitForVblank(void) {
   while (*SCANLINECOUNTER > 160);
   while (*SCANLINECOUNTER < 160);
}

//Do not worry about what DMA is - we will be learning this on Monday's Recitation! 
void drawGhost(int r, int c, int width, int height, const unsigned short *image) {
	for (int x = 0; x < height; x++) {
		DMA[3].src = image + OFFSET(x,0,width);
		DMA[3].dst = videoBuffer + OFFSET(x+r, c, 240);
		DMA[3].cnt = width | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT;
	}
}
