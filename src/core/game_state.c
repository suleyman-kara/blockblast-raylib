#include "game.h"
#include "score.h"
#include <string.h>

void GameInit(GameState *state)
{
    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    BoardInit(&state->board);
    state->highScore = ScoreLoadHigh();
    state->combo = 0;
    state->score = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;
    state->selectedSetting = 0;
    state->selectedLevel = 0;

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        state->slots[i].piece = NULL;

    // Load adventure progress and lives
    AdventureLoadProgress(&state->adventureSave);
    AdventureLoadLives(&state->adventureSave);
}

void GameReset(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state->slots[i].piece);

    BoardInit(&state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);
}

void GameResetAdventure(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state->slots[i].piece);

    // Initialize adventure level (sets up board with obstacles)
    AdventureInitLevel(&state->adventure, state->selectedLevel, &state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);
}