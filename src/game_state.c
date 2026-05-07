#include "game.h"
#include <string.h>
#include <stdio.h>

// ─── High Score Save/Load (moved from deleted score module) ──────────────────
int ScoreLoadHigh(void)
{
    int score = 0;
    FILE *f = fopen("data/highscore.dat", "rb");
    if (f) {
        fread(&score, sizeof(int), 1, f);
        fclose(f);
    }
    return score;
}

void ScoreSaveHigh(int score)
{
    FILE *f = fopen("data/highscore.dat", "wb");
    if (f) {
        fwrite(&score, sizeof(int), 1, f);
        fclose(f);
    }
}

// ─── Game Init ───────────────────────────────────────────────────────────────
void GameInit(GameState *state)
{
    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    BoardInit(&state->board);
    state->highScore = ScoreLoadHigh();
    state->selectedLevel = 0;

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        state->slots[i].piece = NULL;

    // Load level progress
    LevelLoadProgress(state->levelCompleted);
}

// ─── Game Reset (unified classic + adventure) ────────────────────────────────
void GameReset(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state->slots[i].piece);

    // Initialize level (sets up board + prefill)
    LevelInit(&state->level, state->selectedLevel, &state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;

    // Determine gem chances from level definition
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
    float diamondChance = (def->targetDiamonds > 0) ? DIAMOND_SPAWN_CHANCE : 0.0f;
    float emeraldChance = (def->targetEmeralds > 0) ? EMERALD_SPAWN_CHANCE : 0.0f;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance, emeraldChance);
}
