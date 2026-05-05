#include "adventure.h"
#include "board.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// ─── Level Definitions ────────────────────────────────────────────────────────
// Progression:
//   Level 1-2:  GOAL_SCORE       (just reach target score)
//   Level 3-4:  GOAL_GEMS        (collect diamonds only)
//   Level 5-6:  GOAL_MIXED_GEMS  (collect diamonds + emeralds)
//   Level 7-10: GOAL_MIXED_ALL   (diamonds + emeralds + score)
static const LevelDef levelDefs[TOTAL_LEVELS] = {
    {
        .levelNumber = 1,
        .goalType = GOAL_SCORE,
        .targetScore = 500,
        .prefillCount = 0,
        .obstacleCount = 0,
        .description = "500 puana ulas"
    },
    {
        .levelNumber = 2,
        .goalType = GOAL_SCORE,
        .targetScore = 1000,
        .prefillCount = 0,
        .obstacleCount = 0,
        .description = "1000 puana ulas"
    },
    {
        .levelNumber = 3,
        .goalType = GOAL_GEMS,
        .targetDiamonds = 3,
        .gemVariance = 1,
        .prefillCount = 2,
        .obstacleCount = 0,
        .description = "3 elmas topla"
    },
    {
        .levelNumber = 4,
        .goalType = GOAL_GEMS,
        .targetDiamonds = 5,
        .gemVariance = 2,
        .prefillCount = 3,
        .obstacleCount = 0,
        .description = "5 elmas topla"
    },
    {
        .levelNumber = 5,
        .goalType = GOAL_MIXED_GEMS,
        .targetDiamonds = 3,
        .targetEmeralds = 2,
        .gemVariance = 1,
        .prefillCount = 4,
        .obstacleCount = 0,
        .description = "3 elmas + 2 zumrut topla"
    },
    {
        .levelNumber = 6,
        .goalType = GOAL_MIXED_GEMS,
        .targetDiamonds = 4,
        .targetEmeralds = 3,
        .gemVariance = 2,
        .prefillCount = 5,
        .obstacleCount = 0,
        .description = "4 elmas + 3 zumrut topla"
    },
    {
        .levelNumber = 7,
        .goalType = GOAL_MIXED_ALL,
        .targetScore = 500,
        .targetDiamonds = 3,
        .targetEmeralds = 2,
        .gemVariance = 1,
        .prefillCount = 5,
        .obstacleCount = 0,
        .description = "500 puan + 3 elmas + 2 zumrut"
    },
    {
        .levelNumber = 8,
        .goalType = GOAL_MIXED_ALL,
        .targetScore = 800,
        .targetDiamonds = 4,
        .targetEmeralds = 3,
        .gemVariance = 2,
        .prefillCount = 6,
        .obstacleCount = 0,
        .description = "800 puan + 4 elmas + 3 zumrut"
    },
    {
        .levelNumber = 9,
        .goalType = GOAL_MIXED_ALL,
        .targetScore = 1000,
        .targetDiamonds = 5,
        .targetEmeralds = 4,
        .gemVariance = 2,
        .prefillCount = 7,
        .obstacleCount = 0,
        .description = "1000 puan + 5 elmas + 4 zumrut"
    },
    {
        .levelNumber = 10,
        .goalType = GOAL_MIXED_ALL,
        .targetScore = 1500,
        .targetDiamonds = 6,
        .targetEmeralds = 5,
        .gemVariance = 3,
        .prefillCount = 8,
        .obstacleCount = 0,
        .description = "1500 puan + 6 elmas + 5 zumrut (FINAL)"
    },
};

const LevelDef *AdventureGetLevelDefs(void)
{
    return levelDefs;
}

// ─── Board Obstacle Setup + Prefill ──────────────────────────────────────────
void AdventureInitLevel(AdventureState *adventure, int levelIndex, Board *board)
{
    memset(adventure, 0, sizeof(AdventureState));
    adventure->currentLevel = levelIndex;

    const LevelDef *level = &levelDefs[levelIndex];

    // Clear the board
    BoardInit(board);

    // Set goal targets with variance
    adventure->goalScore = level->targetScore;

    if (level->gemVariance > 0) {
        static bool seeded = false;
        if (!seeded) {
            srand((unsigned int)time(NULL));
            seeded = true;
        }
        int variance = rand() % (level->gemVariance * 2 + 1) - level->gemVariance;
        adventure->goalDiamonds = level->targetDiamonds + variance;
        if (adventure->goalDiamonds < 1) adventure->goalDiamonds = 1;

        if (level->targetEmeralds > 0) {
            variance = rand() % (level->gemVariance * 2 + 1) - level->gemVariance;
            adventure->goalEmeralds = level->targetEmeralds + variance;
            if (adventure->goalEmeralds < 1) adventure->goalEmeralds = 1;
        }
    } else {
        adventure->goalDiamonds = level->targetDiamonds;
        adventure->goalEmeralds = level->targetEmeralds;
    }

    // Place obstacles (ice/stone)
    adventure->boardHasObstacles = (level->obstacleCount > 0);
    memset(adventure->iceUnderColor, 0, sizeof(adventure->iceUnderColor));

    for (int i = 0; i < level->obstacleCount; i++) {
        int r = level->obstacles[i].row;
        int c = level->obstacles[i].col;
        if (r >= 0 && r < GRID_SIZE && c >= 0 && c < GRID_SIZE) {
            if (level->isIce[i]) {
                int colorIdx = (levelIndex + i) % 7 + 1;
                board->cells[r][c] = CELL_ICE;
                adventure->iceUnderColor[r][c] = colorIdx;
            } else {
                board->cells[r][c] = CELL_STONE;
            }
        }
    }

    // Pre-fill board with random pieces that may contain gems
    if (level->prefillCount > 0) {
        BoardPrefillGems(board, level->prefillCount, levelIndex);
    }
}

int AdventureGetIceUnderColor(AdventureState *adventure, int row, int col)
{
    return adventure->iceUnderColor[row][col];
}