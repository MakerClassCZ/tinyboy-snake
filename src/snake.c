// ***************************************************************************
//
//                                MakerClass Snake
//                         Core game logic implementation
//
// ***************************************************************************

#include "../include.h"

// Game state
GameState game;

// Direction vectors
const s8 dir_x[] = {1, 0, -1, 0};   // RIGHT, UP, LEFT, DOWN
const s8 dir_y[] = {0, -1, 0, 1};   // RIGHT, UP, LEFT, DOWN

// Initialize game to starting state
void InitGame(void) {
    // Reset game state
    game.length = 3;
    game.direction = DIR_RIGHT;
    game.next_direction = DIR_RIGHT;
    game.score = 0;
    game.tick_counter = 0;
    game.move_delay = 10;               // Initial speed (lower = faster)
    game.state = STATE_PLAYING;

    // Initialize snake in center of grid
    game.snake[0].x = GRID_WIDTH / 2;
    game.snake[0].y = GRID_HEIGHT / 2;
    game.snake[1].x = game.snake[0].x - 1;
    game.snake[1].y = game.snake[0].y;
    game.snake[2].x = game.snake[1].x - 1;
    game.snake[2].y = game.snake[1].y;

    // Spawn first food
    SpawnFood();
}

// Spawn food at random location (not on snake, away from borders and score)
// Returns True if successfully found valid position
Bool SpawnFood(void) {
    Bool valid = False;
    u16 attempts = 0;

    while (!valid) {
        valid = True;
        
        // Spawn food with 1-cell margin from borders
        game.food_x = 1 + RandU8Max(GRID_WIDTH - 3);
        game.food_y = 1 + RandU8Max(GRID_HEIGHT - 3);

        // Check if food spawned in score area (top-left corner)
        if (game.food_x <= 3 && game.food_y == 1) {
            valid = False;
            continue;
        }

        // Check if food spawned on snake
        for (u16 i = 0; i < game.length; i++) {
            if (game.snake[i].x == game.food_x && game.snake[i].y == game.food_y) {
                valid = False;
                break;
            }
        }

        attempts++;
        if (attempts > 100) return False;
    }

    return True;
}

// Check collision with walls or self
Bool CheckCollision(s8 x, s8 y) {
    // Check boundaries (avoid borders: x 0-30, y 0-14)
    if (x < 0 || x > 30 || y < 0 || y > 14) {
        return True;
    }

    // Check self collision (skip head)
    for (u16 i = 1; i < game.length; i++) {
        if (game.snake[i].x == x && game.snake[i].y == y) {
            return True;
        }
    }

    return False;
}

// Handle input from buttons
void HandleInput(void) {
    u8 key = KeyGet();

    if (game.state == STATE_MENU) {
        if (key == KEY_A || key == KEY_B) {
            InitGame();
            Sound(100, 50);
        }
    } else if (game.state == STATE_PLAYING) {
        // Change direction (prevent 180-degree turns)
        if (key == KEY_RIGHT && game.direction != DIR_LEFT) {
            game.next_direction = DIR_RIGHT;
        } else if (key == KEY_LEFT && game.direction != DIR_RIGHT) {
            game.next_direction = DIR_LEFT;
        } else if (key == KEY_UP && game.direction != DIR_DOWN) {
            game.next_direction = DIR_UP;
        } else if (key == KEY_DOWN && game.direction != DIR_UP) {
            game.next_direction = DIR_DOWN;
        }
    } else if (game.state == STATE_GAMEOVER) {
        if (key == KEY_A || key == KEY_B) {
            game.state = STATE_MENU;
            Sound(100, 50);
        }
    }
}

// Update game logic
void UpdateGame(void) {
    if (game.state != STATE_PLAYING) {
        return;
    }

    // Increment tick counter
    game.tick_counter++;

    // Only move snake at certain intervals (controls speed)
    if (game.tick_counter < game.move_delay) {
        return;
    }
    game.tick_counter = 0;

    // Update direction
    game.direction = game.next_direction;

    // Calculate new head position
    s8 new_x = game.snake[0].x + dir_x[game.direction];
    s8 new_y = game.snake[0].y + dir_y[game.direction];

    // Check collision
    if (CheckCollision(new_x, new_y)) {
        game.state = STATE_GAMEOVER;
        Sound(50, 200);                 // Game over sound
        return;
    }

    // Check if food eaten
    Bool ate_food = (new_x == game.food_x && new_y == game.food_y);

    // Save tail position as emergency fallback for food spawn
    s8 tail_x = game.snake[game.length - 1].x;
    s8 tail_y = game.snake[game.length - 1].y;

    if (ate_food) {
        // Increase score
        game.score += 10;

        // Play eating sound
        Sound(150, 30);

        // Increase snake length
        if (game.length < MAX_SNAKE_LENGTH) {
            game.length++;
        }

        // Speed up game slightly
        if (game.move_delay > 3) {
            game.move_delay--;
        }
    }

    // Move snake body
    for (u16 i = game.length - 1; i > 0; i--) {
        game.snake[i].x = game.snake[i - 1].x;
        game.snake[i].y = game.snake[i - 1].y;
    }

    // Move head
    game.snake[0].x = new_x;
    game.snake[0].y = new_y;

    // Spawn new food AFTER snake has moved
    if (ate_food) {
        // Try random spawn
        if (!SpawnFood() && tail_x >= 1 && tail_x <= 30 && tail_y >= 2 && tail_y <= 14) {
            // Emergency fallback - use tail position (user won't notice snake didn't grow)
            game.food_x = tail_x;
            game.food_y = tail_y;
        }
    }
}

// Render menu screen
void RenderMenu(void) {
    // Title screen
    DrawText("MakerClass SNAKE", 2, 8, COL_WHITE);
    DrawText("Press A to", 28, 28, COL_WHITE);
    DrawText("Start", 48, 38, COL_WHITE);

    // Draw simple snake graphic
    for (u8 i = 0; i < 5; i++) {
        if (i < 4) {
            DrawRect(30 + i * 6, 51, 2, 2, COL_WHITE);
        } else {
            DrawRect(30 + i * 6, 50, 4, 4, COL_WHITE);
        }
    }
}

// Render playing screen
void RenderPlaying(void) {
    // Draw border
    DrawFrame(0, 0, WIDTH, HEIGHT, COL_WHITE);

    // Draw snake
    for (u16 i = 0; i < game.length; i++) {
        u8 x = game.snake[i].x * GRID_SIZE + 2;
        u8 y = game.snake[i].y * GRID_SIZE + 2;

        if (i == 0) {
            // Head - bigger
            DrawRect(x, y, GRID_SIZE - 1, GRID_SIZE - 1, COL_WHITE);
        } else {
            // Body
            DrawRect(x + 1, y + 1, GRID_SIZE - 3, GRID_SIZE - 3, COL_WHITE);
        }
    }

    // Draw blinking food
    if ((game.tick_counter / 2) % 2 == 0) {
        u8 fx = game.food_x * GRID_SIZE + 2;
        u8 fy = game.food_y * GRID_SIZE + 2;
        DrawRect(fx, fy, GRID_SIZE - 1, GRID_SIZE - 1, COL_WHITE);
        DrawPoint(fx + 1, fy + 1, COL_BLACK);
    }

    // Draw score
    char score_str[16];
    score_str[0] = '0' + (game.score / 100) % 10;
    score_str[1] = '0' + (game.score / 10) % 10;
    score_str[2] = '0' + game.score % 10;
    score_str[3] = '\0';
    DrawText(score_str, 2, 1, COL_WHITE);
}

// Render game over screen
void RenderGameOver(void) {
    // Game over screen
    DrawText("GAME OVER", 32, 12, COL_WHITE);

    // Display score
    DrawText("Score:", 40, 28, COL_WHITE);
    char score_str[16];
    score_str[0] = '0' + (game.score / 100) % 10;
    score_str[1] = '0' + (game.score / 10) % 10;
    score_str[2] = '0' + game.score % 10;
    score_str[3] = '\0';
    DrawText(score_str, 48, 38, COL_WHITE);

    DrawText("Press A", 40, 52, COL_WHITE);
}

// Render game to display
void RenderGame(void) {
    DrawClear();

    switch (game.state) {
        case STATE_MENU:
            RenderMenu();
            break;

        case STATE_PLAYING:
            RenderPlaying();
            break;

        case STATE_GAMEOVER:
            RenderGameOver();
            break;
    }

    // Update display
    DispUpdate();
}

// Setup function
void setup(void) {
    // Initialize device
    DevInit();

    // Set initial state
    game.state = STATE_MENU;
    game.score = 0;

    // Clear screen
    DrawClear();
    DispUpdate();

    // Play startup sound
    Sound(100, 100);
    WaitMs(50);
    Sound(150, 100);
}

// Main game loop - runs continuously
void loop(void) {
    while (1) {
        // Handle input
        HandleInput();

        // Update game logic
        UpdateGame();

        // Render to screen
        RenderGame();

        // Frame delay (~60 FPS)
        WaitMs(16);
    }
}
