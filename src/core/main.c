#include "raylib.h"
#include "game.h"
#include "input.h"
#include "render.h"
#include "sound.h"
#include "textures.h"

#include <stdio.h>

Font gameFont = { 0 };

int main(void)
{
    // Ensure data directory exists for highscore file
    MakeDirectory("data");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Blast");
    SetExitKey(0);  // Disable ESC as exit key (we use it for settings menu)
    SetTargetFPS(60);

    // Load TrueType font for crisp text rendering
    gameFont = LoadFontEx("assets/fonts/RussoOne-Regular.ttf", 200, 0, 0);
    if (gameFont.texture.id != 0)
        printf("INFO: FONT: Loaded custom font successfully.\n");

    // Load all textures at startup
    TexturesLoad();

    GameState state;
    GameInit(&state);

    // Initialize sound system (must be after InitWindow)
    SoundInit(&state.sound);

    while (!WindowShouldClose()) {
        // Update music stream
        SoundUpdate(&state.sound);

        // Input (during play screens)
        if (state.currentScreen == SCREEN_PLAY || state.currentScreen == SCREEN_ADVENTURE_PLAY) {
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

    // Unload all textures
    TexturesUnload();

    // Unload font
    if (gameFont.texture.id != 0) UnloadFont(gameFont);

    CloseWindow();
    return 0;
}