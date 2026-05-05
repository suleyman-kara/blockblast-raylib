#include "adventure.h"
#include "board.h"
#include <stdio.h>
#include <string.h>

// ─── Goal Progress ────────────────────────────────────────────────────────────
void AdventureUpdateProgress(AdventureState *adventure, Board *board,
                              int linesCleared, int combo, int points,
                              int diamondsCollected, int emeraldsCollected)
{
    (void)linesCleared;
    (void)combo;
    (void)board;

    // Score is tracked externally via state->score, but we also track it here
    // for the mixed all goal type. The score is passed in via 'points' but
    // the actual score is accumulated in GameState.score. We just need to
    // check against goalScore in AdventureCheckGoal.

    // Collect gems
    adventure->collectedDiamonds += diamondsCollected;
    adventure->collectedEmeralds += emeraldsCollected;
}

bool AdventureCheckGoal(AdventureState *adventure)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[adventure->currentLevel];

    switch (level->goalType) {
        case GOAL_SCORE:
            // Score is checked externally via state->score >= goalScore
            // This function is called after score is updated
            return false; // handled externally

        case GOAL_GEMS:
            return adventure->collectedDiamonds >= adventure->goalDiamonds;

        case GOAL_MIXED_GEMS:
            return adventure->collectedDiamonds >= adventure->goalDiamonds &&
                   adventure->collectedEmeralds >= adventure->goalEmeralds;

        case GOAL_MIXED_ALL:
            // Score is checked externally
            return adventure->collectedDiamonds >= adventure->goalDiamonds &&
                   adventure->collectedEmeralds >= adventure->goalEmeralds;

        default:
            return false;
    }
}

bool AdventureCheckFailure(AdventureState *adventure, Board *board, PieceSlot slots[3])
{
    // Level fails when there are no valid moves left
    return !BoardHasValidMove(board, slots);
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