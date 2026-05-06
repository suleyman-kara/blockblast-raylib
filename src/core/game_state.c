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

    // Load adventure progress
    AdventureLoadProgress(&state->adventureSave);
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

    // Initialize adventure level (sets up board with obstacles + prefill)
    AdventureInitLevel(&state->adventure, state->selectedLevel, &state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;

    // Generate pieces with gem chances based on level type
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[state->selectedLevel];

    float diamondChance = 0.0f;
    float emeraldChance = 0.0f;
    if (level->goalType == GOAL_GEMS || level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
        diamondChance = DIAMOND_SPAWN_CHANCE;
        if (level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
            emeraldChance = EMERALD_SPAWN_CHANCE;
        }
    }

    GenerateRandomPiecesWithGems(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance, emeraldChance);

    // Add float text for level start
    const char *startMsg = (level->goalType == GOAL_SCORE) ? "Collect the target score!" : "Collect the target diamonds!";
    FloatTextAdd(&state->floatTexts, startMsg,
                 SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 80.0f,
                 28, (Color){255, 255, 100, 255});
}
