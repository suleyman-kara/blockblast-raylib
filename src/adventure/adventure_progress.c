#include "adventure.h"
#include "board.h"
#include <stdio.h>
#include <string.h>

// ─── Goal Progress ────────────────────────────────────────────────────────────
void AdventureUpdateProgress(AdventureState *adventure, Board *board,
                              int linesCleared, int combo, int points)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[adventure->currentLevel];

    // First goal
    switch (level->goalType) {
        case GOAL_CLEAR_LINES:
            adventure->goalProgress += linesCleared;
            break;
        case GOAL_SCORE:
            adventure->goalProgress += points;
            break;
        case GOAL_COMBO:
            if (combo >= 2)
                adventure->goalProgress++;
            break;
        case GOAL_MIXED:
            adventure->goalProgress += linesCleared + points;
            break;
    }

    // Second goal (if goalValue2 > 0, there's a secondary goal)
    if (level->goalValue2 > 0) {
        switch (level->goalType2) {
            case GOAL_CLEAR_LINES:
                adventure->goalProgress2 += linesCleared;
                break;
            case GOAL_SCORE:
                adventure->goalProgress2 += points;
                break;
            case GOAL_COMBO:
                if (combo >= 2)
                    adventure->goalProgress2++;
                break;
            default:
                break;
        }
    }

    // Count combos for the combo goal
    if (combo >= 2) {
        adventure->comboCount++;
    }

    (void)board; // unused
}

bool AdventureCheckGoal(AdventureState *adventure)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[adventure->currentLevel];

    // Check primary goal
    bool primaryMet = adventure->goalProgress >= level->goalValue;

    // Check secondary goal if it exists (goalValue2 > 0 and goalType2 is set)
    bool secondaryMet = true;
    if (level->goalValue2 > 0) {
        secondaryMet = adventure->goalProgress2 >= level->goalValue2;
    }

    return primaryMet && secondaryMet;
}

bool AdventureCheckFailure(AdventureState *adventure)
{
    // Check if pieceLimit is reached (handled externally)
    // This returns true if goal was NOT reached and pieces are exhausted
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[adventure->currentLevel];
    if (adventure->piecesUsed >= level->pieceLimit) {
        return !AdventureCheckGoal(adventure);
    }
    return false;
}

// ─── Star Calculation ─────────────────────────────────────────────────────────
int AdventureCalcStars(AdventureState *adventure)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[adventure->currentLevel];

    float progress;
    if (level->goalValue2 > 0) {
        // Use average of the two progress percentages
        float p1 = (float)adventure->goalProgress / (float)level->goalValue;
        float p2 = (float)adventure->goalProgress2 / (float)level->goalValue2;
        progress = (p1 + p2) / 2.0f;
    } else {
        progress = (float)adventure->goalProgress / (float)level->goalValue;
    }

    if (progress >= STAR_3_THRESHOLD) return 3;
    if (progress >= STAR_2_THRESHOLD) return 2;
    if (progress >= STAR_1_THRESHOLD) return 1;
    return 0;
}

// ─── Progress Save/Load ──────────────────────────────────────────────────────
void AdventureLoadProgress(AdventureSaveData *save)
{
    FILE *f = fopen("data/adventure_progress.dat", "rb");
    if (f) {
        fread(save->levels, sizeof(LevelProgress), TOTAL_LEVELS, f);
        fclose(f);
    } else {
        // Default: only level 1 is unlocked
        for (int i = 0; i < TOTAL_LEVELS; i++) {
            save->levels[i].unlocked = (i == 0);
            save->levels[i].bestStars = 0;
            save->levels[i].completed = false;
        }
    }
}

void AdventureSaveProgress(AdventureSaveData *save)
{
    FILE *f = fopen("data/adventure_progress.dat", "wb");
    if (f) {
        fwrite(save->levels, sizeof(LevelProgress), TOTAL_LEVELS, f);
        fclose(f);
    }
}