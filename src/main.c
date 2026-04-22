#include "raylib.h"
#include "game.h"
#include "input.h"
#include "render.h"
#include "sound.h"

#include <stdio.h>

int main(void)
{
    // Ensure data directory exists for highscore file
    MakeDirectory("data");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Blast");
    SetExitKey(0);  // Disable ESC as exit key (we use it for settings menu)
    SetTargetFPS(60);

    GameState state;
    GameInit(&state);

    // Initialize sound system (must be after InitWindow)
    SoundInit(&state.sound);

    while (!WindowShouldClose()) {
        // Update music stream
        SoundUpdate(&state.sound);

        // Input (only during play screen)
        if (state.currentScreen == SCREEN_PLAY) {
            InputUpdate(&state);
        }

        // Update
        GameUpdate(&state);

        // Draw
        RenderFrame(&state);
    }

    // Cleanup: free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state.slots[i].piece);

    // Cleanup sound system
    SoundClose(&state.sound);

    CloseWindow();
    return 0;
}