#include "gba.h"
#include "logic.h"
#include "graphics.h"
// TA-TODO: Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
//#include "images/garbage.h"
#include "images/hogwarts.h"
#include "images/begin0.h"
#include "images/apple.h"
#include "images/hedwig.h"
#include "images/parchment.h"
#include "images/win.h"
#include "images/lose.h"

#include <stdio.h>
#include <stdlib.h>

// AppState enum definition
typedef enum {
    // TA-TODO: Add any additional states you need for your app.
    START,
    START_NODRAW,
	APP_FEED,
    APP_INIT,
    APP,
    APP_EXIT,
	APP_WIN,
    APP_EXIT_NODRAW,
} GBAState;


int main(void) {
    // TA-TODO: Manipulate REG_DISPCNT here to set Mode 3.
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    GBAState state = START;

    // We store the "previous" and "current" states.
    AppState currentAppState, nextAppState;

    // We store the current and previous values of the button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

	//parameters for apple
	Coordinate appleP = {40, 180};
	int appleSize = 15;
	//parameters for falling gates
	//int gateSize = 40;

    while(1) {
        // Load the current state of the buttons
        currentButtons = BUTTONS;	
		waitForVBlank();
		if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
			state = START;
		}

        // TA-TODO: Manipulate the state machine below as needed.
        switch(state) {
        case START:
            // Wait for VBlank
            waitForVBlank();

            // TA-TODO: Draw the start state here.
			drawFullScreenImageDMA(hogwarts);

            state = START_NODRAW;
            break;
        case START_NODRAW:
            // TA-TODO: Check for a button press here to start the app.
            // Start the app by switching the state to APP_INIT.
			//if (KEY_DOWN(BUTTON_A, BUTTONS) && !KEY_DOWN(previousButtons, BUTTONS)) {

			if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
				//printf("what happened?\n");
				drawFullScreenImageDMA(begin0);
				drawImageDMA(appleP.row, appleP.col, appleSize, appleSize, apple);
				state = APP_FEED;
			}
			break;
		case APP_FEED:
			if (appleP.row <= 120 && appleP.row >= 60 && appleP.col <= 100 && appleP.col >= 70) {
				waitForVBlank();
				fillScreenDMA(WHITE);
				drawImageDMA(5, 5, 150, 150, parchment);
				drawImageDMA(60, 160, 56, 80, hedwig);
				char *letter = "Congrats!";
				//drawChar(40, 40, 'C', BLACK);
				drawString(20, 20, letter, BLACK);
				char *line2 = "You are chosen to be";
				drawString(20, 30, line2, BLACK);
				char *line3 = "A WIZARD!!!";
				drawString(20, 40, line3, BLACK);
				char *line4 = "But first,";
				drawString(20, 50, line4, BLACK);
				char *line5 = "You need to pass a";
				drawString(20, 60, line5, BLACK);
				char *line6 = "test to get through";
				drawString(20, 70, line6, BLACK);
				char *line7 = "9 3/4 platform.";
				drawString(20, 80, line7, BLACK);
				char *line8 = "Press [ARROW] to";
				drawString(20, 90, line8, BLACK);
				char *line9 = "match the gates'";
				drawString(20, 100, line9, BLACK);
				char *line10 = "directions.";
				drawString(20, 110, line10, BLACK);
				char *line11 = "Press [ENTER] to";
				drawString(20, 120, line11, BLACK);
				char *line12 = "start";
				drawString(20, 130, line12, BLACK);
				state = APP_INIT;
				appleP.row = 40;
				appleP.col = 180;
				break;
			}
			if (KEY_JUST_PRESSED(BUTTON_RIGHT, currentButtons, previousButtons)) {
		   		appleP.col+=2; 
				//drawFullScreenImageDMA(begin0);
				drawImageDMA(appleP.row, appleP.col, appleSize, appleSize, apple);
				drawRectangle(appleP.row, appleP.col-2, 2, appleSize, begin0);
				break;
			}
			if (KEY_JUST_PRESSED(BUTTON_LEFT, currentButtons, previousButtons)) {
		   		appleP.col-=2;
				//drawFullScreenImageDMA(begin0);
				drawImageDMA(appleP.row, appleP.col, appleSize, appleSize, apple);
				drawRectangle(appleP.row, appleP.col+appleSize, 2, appleSize, begin0);
				break;
			}
			if (KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
		    	appleP.row-=2;
				//drawFullScreenImageDMA(begin0);
				drawImageDMA(appleP.row, appleP.col, appleSize, appleSize, apple);
				drawRectangle(appleSize+appleP.row, appleP.col, appleSize, 2, begin0);
				break;
			}
			if (KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) {
		    	appleP.row+=2; 
				//drawFullScreenImageDMA(begin0);
				drawImageDMA(appleP.row, appleP.col, appleSize, appleSize, apple);
				drawRectangle(appleP.row-2, appleP.col, appleSize, 2, begin0);
			}
			break;
            
        case APP_INIT:
            // Initialize the app. Switch to the APP state.
			
            initializeAppState(&currentAppState);

            // Draw the initial state of the app
            //fullDrawAppState(&currentAppState);

			if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
				//draw the owl then change to state APP when box start to falling
				fullDrawAppState(&currentAppState);
				state = APP;
			}
            break;
        case APP:
            // Process the app for one frame, store the next state
            nextAppState = processAppState(&currentAppState, previousButtons, currentButtons);
			//nextAppState = currentAppState;
            // Wait for VBlank before we do any drawing.
            waitForVBlank();

            // Undraw the previous state
            undrawAppState(&currentAppState);

			//drawImageDMA(0, 0, 56, 80, hedwig);
            // Draw the current state
            drawAppState(&nextAppState);

            // Now set the current state as the next state for the next iter.
            currentAppState = nextAppState;

            // Check if the app is exiting. If it is, then go to the exit state.
            if (nextAppState.gameOver) state = APP_EXIT;
			if (nextAppState.win) state = APP_WIN;
            break;
        case APP_EXIT:
            // Wait for VBlank
            waitForVBlank();

            // TA-TODO: Draw the exit / gameover screen
			drawFullScreenImageDMA(lose);
            state = APP_EXIT_NODRAW;
            break;
		case APP_WIN:

        	waitForVBlank();
        	drawFullScreenImageDMA(win);
        	state = APP_EXIT_NODRAW;
        	break;

        case APP_EXIT_NODRAW:
            // TA-TODO: Check for a button press here to go back to the start screen
			if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
				state = START;
			}
            break;
        }

        // Store the current state of the buttons
        previousButtons = currentButtons;
    }

    return 0;
}
