#include "logic.h"
#include "images/begin0.h"




void initializeAppState(AppState* appState) {
    // TA-TODO: Initialize everything that's part of this AppState struct here.
    // Suppose the struct contains random values, make sure everything gets
    // the value it should have when the app begins.
	appState->gameOver = 0;
	appState->win = 0;

	int r = randint(0, 3);
	Gate gateNow = {0, 40*r, r};
	appState->fallingGate = gateNow;
	appState->round = 0;
}

// TA-TODO: Add any process functions for sub-elements of your app here.
// For example, for a snake game, you could have a processSnake function
// or a createRandomFood function or a processFoods function.
//
// e.g.:
// static Snake processSnake(Snake* currentSnake);
// static void generateRandomFoods(AppState* currentAppState, AppState* nextAppState);

// This function processes your current app state and returns the new (i.e. next)
// state of your application.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow) {
    /* TA-TODO: Do all of your app processing here. This function gets called
     * every frame.
     *
     * To check for key presses, use the KEY_JUST_PRESSED macro for cases where
     * you want to detect each key press once, or the KEY_DOWN macro for checking
     * if a button is still down.
     *
     * To count time, suppose that the GameBoy runs at a fixed FPS (60fps) and
     * that VBlank is processed once per frame. Use the vBlankCounter variable
     * and the modulus % operator to do things once every (n) frames. Note that
     * you want to process button every frame regardless (otherwise you will
     * miss inputs.)
     *
     * Do not do any drawing here.
     *
     * TA-TODO: VERY IMPORTANT! READ THIS PART.
     * You need to perform all calculations on the currentAppState passed to you,
     * and perform all state updates on the nextAppState state which we define below
     * and return at the end of the function. YOU SHOULD NOT MODIFY THE CURRENTSTATE.
     * Modifying the currentAppState will mean the undraw function will not be able
     * to undraw it later.
     */

    AppState nextAppState = *currentAppState;
	Gate currentGate = nextAppState.fallingGate;
	currentGate.row+=2;
	int gateID = currentGate.id;
	nextAppState.fallingGate = currentGate;
	//nextAppState.round++;
	int r = randint(0, 3);
	Gate gateNow = {0, 40*r, r};
	if (KEY_JUST_PRESSED(BUTTON_RIGHT, keysPressedNow, keysPressedBefore) && gateID == 0) {
		nextAppState.round++;
		
		nextAppState.fallingGate = gateNow;
	}

	if (KEY_JUST_PRESSED(BUTTON_LEFT, keysPressedNow, keysPressedBefore) && gateID == 1) {
		nextAppState.round++;
		nextAppState.fallingGate = gateNow;
	}

	if (KEY_JUST_PRESSED(BUTTON_UP, keysPressedNow, keysPressedBefore) && gateID == 2) {
		nextAppState.round++;
		nextAppState.fallingGate = gateNow;
	}

	if (KEY_JUST_PRESSED(BUTTON_DOWN, keysPressedNow, keysPressedBefore) && gateID == 3) {
		nextAppState.round++;
		nextAppState.fallingGate = gateNow;
	}

	
	if (currentGate.row >=160) nextAppState.gameOver = 1;
	if (nextAppState.round == 10) nextAppState.win = 1;
    //UNUSED(keysPressedBefore);
    //UNUSED(keysPressedNow);

    return nextAppState;
}
