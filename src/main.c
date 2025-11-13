// ***************************************************************************
//
//                        TSnake - Nokia Snake Game
//                    Main entry point and initialization
//
// ***************************************************************************

#include "../include.h"

// Sound function - plays a tone for specified duration
void Sound(u8 freq, u8 dur) {
    if (freq == 0) {
        WaitMs(dur);
    } else {
        int n = (510 - 2 * freq);
        PlayTone(n - 1);
        WaitUs(n * dur + n * dur / 2);
        StopSound();
    }
}

// Main entry point
int main(void) {
    setup();        // Initialize game
    loop();         // Main game loop (never returns)
    return 0;
}
