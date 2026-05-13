#include "level.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LEVELS_FILE "data/levels.txt"
#define PROGRESS_FILE "data/progress.txt"

// ─── Level Definitions ────────────────────────────────────────────────────────
// Index 0 = classic mode (infinite, no targets)
// Index 1-10 = adventure levels
static const LevelDef defaultLevelDefs[TOTAL_LEVELS + 1] = {
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

static LevelDef levelDefs[TOTAL_LEVELS + 1];

const LevelDef *LevelGetDefs(void)
{
    return levelDefs;
}

static void LevelUseDefaults(void)
{
    memcpy(levelDefs, defaultLevelDefs, sizeof(levelDefs));
}

static void LevelWriteExampleFile(void)
{
    FILE *f = fopen(LEVELS_FILE, "w");
    if (!f) return;

    fprintf(f, "# level targetScore targetDiamonds targetEmeralds prefillCount\n");
    fprintf(f, "# 0 means this target is disabled.\n");
    for (int i = 1; i <= TOTAL_LEVELS; i++) {
        const LevelDef *def = &levelDefs[i];
        fprintf(f, "%d %d %d %d %d\n",
                def->level,
                def->targetScore,
                def->targetDiamonds,
                def->targetEmeralds,
                def->prefillCount);
    }

    fclose(f);
}

void LevelLoadDefinitions(void)
{
    char line[160];
    FILE *f;

    LevelUseDefaults();

    f = fopen(LEVELS_FILE, "r");
    if (!f) {
        LevelWriteExampleFile();
        return;
    }

    while (fgets(line, sizeof(line), f)) {
        LevelDef def;
        int readCount;

        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        readCount = sscanf(line, "%d %d %d %d %d",
                           &def.level,
                           &def.targetScore,
                           &def.targetDiamonds,
                           &def.targetEmeralds,
                           &def.prefillCount);
        if (readCount != 5)
            continue;
        if (def.level < 1 || def.level > TOTAL_LEVELS)
            continue;
        if (def.targetScore < 0 || def.targetDiamonds < 0 ||
            def.targetEmeralds < 0 || def.prefillCount < 0)
            continue;

        levelDefs[def.level] = def;
    }

    fclose(f);
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

    FILE *f = fopen(PROGRESS_FILE, "r");
    if (f) {
        char line[80];
        while (fgets(line, sizeof(line), f)) {
            int level = 0;
            int done = 0;

            if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
                continue;

            if (sscanf(line, "%d %d", &level, &done) == 2) {
                if (level >= 1 && level <= TOTAL_LEVELS)
                    completed[level - 1] = (done != 0);
            } else if (sscanf(line, "%d", &level) == 1) {
                if (level >= 1 && level <= TOTAL_LEVELS)
                    completed[level - 1] = true;
            }
        }
        fclose(f);
    }
}

void LevelSaveProgress(bool completed[TOTAL_LEVELS])
{
    FILE *f = fopen(PROGRESS_FILE, "w");
    if (f) {
        fprintf(f, "# level completed\n");
        for (int i = 0; i < TOTAL_LEVELS; i++) {
            fprintf(f, "%d %d\n", i + 1, completed[i] ? 1 : 0);
        }
        fclose(f);
    }
}
