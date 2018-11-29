#include "gba.h"
#include "tl06.h"
#include "snoopframes.h"
#include "didnotdraw.h"
#include <stdio.h>

#define FRAME_COUNT 60
#define FLIP_STEP 15

int main(void)
{
    int count = 0;
    const u16 *frame;
    char frame_str[25];
    int flag_pause = 1;
    int first_frame = 1;

    // Set GBA mode 3
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    // Handle button presses
    u32 previousButtons = ~BUTTONS;
    u32 currentButtons  = BUTTONS;

    while(1)
    {
        currentButtons = BUTTONS;

        // Prepare the data: get the frame and the frame counter
        frame = getPointerToCurrentFrame(snoopframes, SNOOPFRAMES_WIDTH, count);
        sprintf(frame_str, "Frame %d %c", count, 0x80 + count / FLIP_STEP);

        // Wait two vblanks so that the animation takes some time
        waitForVBlank();
        waitForVBlank();

        // Don't try to copy this buddy, it won't help you at all
        DMA[3].src = didnotdraw;
        DMA[3].dst = videoBuffer + 0x12c0;
        DMA[3].cnt = 0x840003c0U;

        if (count < FLIP_STEP)
            drawSquareImage(frame, SNOOPFRAMES_WIDTH);
        else if (count < 2 * FLIP_STEP)
            drawSquareImageFlippedHorizontal(frame, SNOOPFRAMES_WIDTH);
        else if (count < 3 * FLIP_STEP)
            drawSquareImageFlippedBoth(frame, SNOOPFRAMES_WIDTH);
        else
            drawSquareImageFlippedVertical(frame, SNOOPFRAMES_WIDTH);

        // Are we changing play/pause state?
        if (first_frame || KEY_PRESSED(BUTTON_A, previousButtons, currentButtons)) {
            // Update the state
            flag_pause = !flag_pause;

            // Clear the help text area
            drawRectDMA(170, 0, 70, 160, BLACK);

            // Write the help text
            if (flag_pause) {
                drawString(170, 50, "Hit A (Z)", WHITE);
                drawString(170, 60, "to play.", WHITE);

                drawString(170, 80, "Hit L (A)", WHITE);
                drawString(170, 90, "for prev.", WHITE);
                drawString(170, 100, "frame.", WHITE);

                drawString(170, 120, "Hit R (S)", WHITE);
                drawString(170, 130, "for next", WHITE);
                drawString(170, 140, "frame.", WHITE);
            } else {
                drawString(170, 50, "Hit A (Z)", WHITE);
                drawString(170, 60, "to pause.", WHITE);
            }
        }

        // If we're paused, allow for frame forwarding - rewinding
        if (flag_pause) {
            if (KEY_PRESSED(BUTTON_L, previousButtons, currentButtons))
                count = (count - 1 + FRAME_COUNT) % FRAME_COUNT;
            else if (KEY_PRESSED(BUTTON_R, previousButtons, currentButtons))
                count = (count + 1) % FRAME_COUNT;
        }

        // Write the frame counter
        drawRectDMA(170, 0, 70, 20, BLACK);
        drawString(170, 5, frame_str, WHITE);

        // If not paused, skip to the next frame.
        if (!flag_pause) count = (count + 1) % FRAME_COUNT;

        // Update the previousButtons
        previousButtons = currentButtons;

        // Can't be the first pass anymore.
        first_frame = 0;
    }

    return 0;
}
