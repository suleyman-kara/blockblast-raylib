#include "level.h"
#include "board.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// ─── Level Definitions ────────────────────────────────────────────────────────
// Index 0 = classic mode (infinite, no targets)
// Index 1-10 = adventure levels
static const LevelDef levelDefs[TOTAL_LEVELS + 1] = {
    // Classic mode — no targets, no prefill
    { .level = 0,  .targetScore = 0, .targetDiamonds = 0, .targetEmeralds = 0, .prefillCount = 0 },

    // Adventure levels
    { .level = 1,  .targetScore = 500,  .prefillCount = 0 },
    { .level = 2,  .targetScore = 1000, .prefillCount = 0 },
    { .level = 3,  .targetDiamonds = 3, .prefillCount = 2 },
    { .level = 4,  .targetDiamonds = 5, .prefillCount = 3 },
    { .level = 5,  .targetDiamonds = 3, .targetEmeralds = 2, .prefillCount = 4 },
    { .level = 6,  .targetDiamonds = 4, .targetEmeralds = 3, .prefillCount = 5 },
    { .level = 7,  .targetScore = 500,  .targetDiamonds = 3, .targetEmeralds = 2, .prefillCount = 5 },
    { .level = 8,  .targetScore = 800,  .targetDiamonds = 4, .targetEmeralds = 3, .prefillCount = 6 },
    { .level = 9,  .targetScore = 1000, .targetDiamonds = 5, .targetEmeralds = 4, .prefillCount = 7 },
    { .level = 10, .targetScore = 1500, .targetDiamonds = 6, .targetEmeralds = 5, .prefillCount = 8 },
};

const LevelDef *LevelGetDefs(void)
{
    return levelDefs;
}

// ─── Level Init ───────────────────────────────────────────────────────────────
void LevelInit(LevelState *state, int levelIndex, Board *board)
{
    memset(state, 0, sizeof(LevelState));
    state->currentLevel = levelIndex;

    BoardInit(board);

    const LevelDef *level = &levelDefs[levelIndex];

    // Pre-fill board with random pieces (adventure levels only)
    if (level->prefillCount > 0) {
        BoardPrefillGems(board, level->prefillCount, levelIndex);
    }
}

// ─── Goal Check ───────────────────────────────────────────────────────────────
bool LevelCheckGoal(LevelState *state, int currentScore)
{
    const LevelDef *level = &levelDefs[state->currentLevel];

    // Classic mode never wins
    if (level->level == 0) return false;

    // Check each target — if set (> 0) and not met, return false
    if (level->targetScore > 0 && currentScore < level->targetScore)
        return false;
    if (level->targetDiamonds > 0 && state->collectedDiamonds < level->targetDiamonds)
        return false;
    if (level->targetEmeralds > 0 && state->collectedEmeralds < level->targetEmeralds)
        return false;

    return true;
}

// ─── Failure Check ────────────────────────────────────────────────────────────
bool LevelCheckFailure(Board *board, PieceSlot slots[3])
{
    return !BoardHasValidMove(board, slots);
}

// ─── Progress Save/Load ───────────────────────────────────────────────────────
void LevelLoadProgress(bool completed[TOTAL_LEVELS])
{
    // Default: nothing completed
    memset(completed, 0, sizeof(bool) * TOTAL_LEVELS);

    FILE *f = fopen("data/level_progress.dat", "rb");
    if (f) {
        fread(completed, sizeof(bool), TOTAL_LEVELS, f);
        fclose(f);
    }
}

void LevelSaveProgress(bool completed[TOTAL_LEVELS])
{
    FILE *f = fopen("data/level_progress.dat", "wb");
    if (f) {
        fwrite(completed, sizeof(bool), TOTAL_LEVELS, f);
        fclose(f);
    }
}
