// ***************************************************************************
//
//                           MakerClass Snake
//                         Snake game header file
//
// ***************************************************************************

#ifndef _SNAKE_H
#define _SNAKE_H

// Game constants
#define GRID_SIZE           4               // Size of each grid cell in pixels
#define GRID_WIDTH          32              // Grid width in cells (128/4)
#define GRID_HEIGHT         16              // Grid height in cells (64/4)
#define MAX_SNAKE_LENGTH    128             // Maximum snake length (playable area is 30x13=390)

// Game states
#define STATE_MENU          0               // Title screen
#define STATE_PLAYING       1               // Game in progress
#define STATE_GAMEOVER      2               // Game over screen

// Direction constants
#define DIR_RIGHT           0
#define DIR_UP              1
#define DIR_LEFT            2
#define DIR_DOWN            3

// Snake segment structure
typedef struct {
    s8 x;
    s8 y;
} SnakeSegment;

// Game state structure
typedef struct {
    SnakeSegment snake[MAX_SNAKE_LENGTH];   // Snake body
    u16 length;                              // Current snake length
    u8 direction;                            // Current direction
    u8 next_direction;                       // Queued direction change
    s8 food_x;                               // Food X position
    s8 food_y;                               // Food Y position
    u16 score;                               // Current score
    u8 state;                                // Game state
    u16 tick_counter;                        // Frame counter for speed control
    u16 move_delay;                          // Frames between moves
} GameState;

// External game state
extern GameState game;

// Function declarations
void InitGame(void);
void UpdateGame(void);
void RenderGame(void);
void RenderMenu(void);
void RenderPlaying(void);
void RenderGameOver(void);
Bool SpawnFood(void);
Bool CheckCollision(s8 x, s8 y);
void HandleInput(void);
void ScoreToStr(u16 score, char* str);

#endif // _SNAKE_H
