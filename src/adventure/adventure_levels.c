#include "adventure.h"
#include "board.h"
#include <string.h>

// ─── Level Definitions ────────────────────────────────────────────────────────
static const LevelDef levelDefs[TOTAL_LEVELS] = {
    {
        .levelNumber = 1,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 3,
        .pieceLimit = 20,
        .obstacleCount = 0,
        .description = "3 satir/sutun temizle"
    },
    {
        .levelNumber = 2,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 5,
        .pieceLimit = 18,
        .obstacleCount = 0,
        .description = "5 satir/sutun temizle"
    },
    {
        .levelNumber = 3,
        .goalType = GOAL_SCORE,
        .goalValue = 500,
        .pieceLimit = 20,
        .obstacleCount = 0,
        .description = "500 puan topla"
    },
    {
        .levelNumber = 4,
        .goalType = GOAL_COMBO,
        .goalValue = 2,
        .pieceLimit = 22,
        .obstacleCount = 0,
        .description = "2 kombo yap (tek hamlede 2+ cizgi)"
    },
    {
        .levelNumber = 5,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 8,
        .pieceLimit = 20,
        .obstacleCount = 0,
        .description = "8 satir/sutun temizle"
    },
    {
        .levelNumber = 6,
        .goalType = GOAL_SCORE,
        .goalValue = 1000,
        .pieceLimit = 22,
        .obstacleCount = 0,
        .description = "1000 puan topla"
    },
    {
        .levelNumber = 7,
        .goalType = GOAL_COMBO,
        .goalValue = 3,
        .pieceLimit = 25,
        .obstacleCount = 0,
        .description = "3 kombo yap"
    },
    {
        .levelNumber = 8,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 6,
        .pieceLimit = 18,
        .obstacleCount = 2,
        .obstacles = {{2, 3}, {5, 4}},
        .isIce = {true, true},
        .description = "6 satir/sutun temizle (buz engelli)"
    },
    {
        .levelNumber = 9,
        .goalType = GOAL_SCORE,
        .goalValue = 800,
        .goalType2 = GOAL_COMBO,
        .goalValue2 = 2,
        .pieceLimit = 20,
        .obstacleCount = 3,
        .obstacles = {{1, 2}, {3, 5}, {6, 1}},
        .isIce = {true, true, true},
        .description = "800 puan + 2 kombo (buz engelli)"
    },
    {
        .levelNumber = 10,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 10,
        .pieceLimit = 22,
        .obstacleCount = 4,
        .obstacles = {{1, 7}, {3, 2}, {4, 5}, {6, 3}},
        .isIce = {true, true, true, true},
        .description = "10 satir/sutun temizle (buz engelli)"
    },
    {
        .levelNumber = 11,
        .goalType = GOAL_SCORE,
        .goalValue = 2000,
        .pieceLimit = 20,
        .obstacleCount = 4,
        .obstacles = {{1, 1}, {2, 5}, {4, 3}, {6, 6}},
        .isIce = {true, true, true, false},
        .description = "2000 puan (buz + tas engelli)"
    },
    {
        .levelNumber = 12,
        .goalType = GOAL_COMBO,
        .goalValue = 4,
        .pieceLimit = 25,
        .obstacleCount = 6,
        .obstacles = {{0, 3}, {2, 6}, {3, 2}, {5, 5}, {6, 1}, {7, 4}},
        .isIce = {true, true, false, true, true, false},
        .description = "4 kombo (buz + tas engelli)"
    },
    {
        .levelNumber = 13,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 12,
        .pieceLimit = 22,
        .obstacleCount = 7,
        .obstacles = {{1, 0}, {1, 7}, {3, 3}, {4, 5}, {5, 1}, {6, 6}, {7, 2}},
        .isIce = {true, true, true, true, true, false, true},
        .description = "12 satir/sutun temizle (buz + tas)"
    },
    {
        .levelNumber = 14,
        .goalType = GOAL_SCORE,
        .goalValue = 3500,
        .pieceLimit = 25,
        .obstacleCount = 8,
        .obstacles = {{0, 2}, {1, 5}, {2, 1}, {3, 7}, {4, 3}, {5, 6}, {6, 0}, {7, 4}},
        .isIce = {true, true, true, false, true, true, false, true},
        .description = "3500 puan (buz + tas engelli)"
    },
    {
        .levelNumber = 15,
        .goalType = GOAL_CLEAR_LINES,
        .goalValue = 15,
        .goalType2 = GOAL_COMBO,
        .goalValue2 = 3,
        .pieceLimit = 30,
        .obstacleCount = 9,
        .obstacles = {{0, 6}, {1, 1}, {2, 4}, {3, 0}, {4, 7}, {5, 2}, {6, 5}, {7, 0}, {7, 7}},
        .isIce = {true, true, true, false, true, true, true, false, false},
        .description = "15 satir + 3 kombo (FINAL BOSS)"
    },
};

const LevelDef *AdventureGetLevelDefs(void)
{
    return levelDefs;
}

// ─── Board Obstacle Setup ─────────────────────────────────────────────────────
void AdventureInitLevel(AdventureState *adventure, int levelIndex, Board *board)
{
    memset(adventure, 0, sizeof(AdventureState));
    adventure->currentLevel = levelIndex;
    adventure->boardHasObstacles = false;

    const LevelDef *level = &levelDefs[levelIndex];

    // Clear the board
    memset(board->cells, CELL_EMPTY, sizeof(board->cells));
    memset(adventure->iceUnderColor, 0, sizeof(adventure->iceUnderColor));

    // Place obstacles
    if (level->obstacleCount > 0) {
        adventure->boardHasObstacles = true;
        for (int i = 0; i < level->obstacleCount; i++) {
            int r = level->obstacles[i].row;
            int c = level->obstacles[i].col;
            if (r >= 0 && r < GRID_SIZE && c >= 0 && c < GRID_SIZE) {
                if (level->isIce[i]) {
                    // Ice: assign a random color underneath
                    int colorIdx = (levelIndex + i) % 7 + 1;  // deterministic color
                    board->cells[r][c] = CELL_ICE;
                    adventure->iceUnderColor[r][c] = colorIdx;
                } else {
                    // Stone: permanent blocker
                    board->cells[r][c] = CELL_STONE;
                }
            }
        }
    }
}

bool AdventureIsCellBlocked(AdventureState *adventure, int row, int col)
{
    (void)adventure;
    return false;  // board.c handles this via BoardCanPlace checking values
}

int AdventureGetIceUnderColor(AdventureState *adventure, int row, int col)
{
    return adventure->iceUnderColor[row][col];
}