#ifndef ADVENTURE_H
#define ADVENTURE_H

#include "core/defs.h"
#include "core/board.h"
#include "core/piece.h"
#include <stdbool.h>

// ─── Goal types ───────────────────────────────────────────────────────────────
typedef enum {
    GOAL_SCORE,         // Level 1-2: reach target score
    GOAL_GEMS,          // Level 3-4: collect diamonds only
    GOAL_MIXED_GEMS,    // Level 5-6: collect diamonds + emeralds
    GOAL_MIXED_ALL,     // Level 7-10: collect diamonds + emeralds + reach score
} GoalType;

// ─── Obstacle position ────────────────────────────────────────────────────────
typedef struct {
    int row;
    int col;
} ObstaclePos;

// ─── Level definition ─────────────────────────────────────────────────────────
typedef struct {
    int levelNumber;
    GoalType goalType;

    // Score goal (GOAL_SCORE, GOAL_MIXED_ALL)
    int targetScore;

    // Gem goals (GOAL_GEMS, GOAL_MIXED_GEMS, GOAL_MIXED_ALL)
    int targetDiamonds;     // base diamond target
    int targetEmeralds;     // base emerald target
    int gemVariance;        // ±variance applied at level start

    // Board prefill
    int prefillCount;       // how many random pieces to place on board at start

    // Obstacles (ice/stone)
    int obstacleCount;
    ObstaclePos obstacles[MAX_OBSTACLES];
    bool isIce[MAX_OBSTACLES];

    const char *description;
} LevelDef;

// ─── Progress data (saved to file) ──────────────────────────────────────────
typedef struct {
    bool unlocked;
    bool completed;
} LevelProgress;

typedef struct {
    LevelProgress levels[TOTAL_LEVELS];
} AdventureSaveData;

// ─── Adventure state (runtime) ──────────────────────────────────────────────
typedef struct {
    int currentLevel;       // 0-based index into levelDefs

    // Goal tracking
    int goalScore;          // actual score target (after variance)
    int goalDiamonds;       // actual diamond target (after variance)
    int goalEmeralds;       // actual emerald target (after variance)

    int collectedDiamonds;
    int collectedEmeralds;

    bool levelComplete;
    bool levelFailed;
    bool showResultScreen;
    float resultTimer;

    // Board obstacle data
    bool boardHasObstacles;
    int iceUnderColor[GRID_SIZE][GRID_SIZE];
} AdventureState;

// ─── Public functions ──────────────────────────────────────────────────────

// Get the level definitions array
const LevelDef *AdventureGetLevelDefs(void);

// Initialize adventure state for a given level (loads board obstacles + prefill)
void AdventureInitLevel(AdventureState *adventure, int levelIndex, Board *board);

// Update goal progress after a piece placement + line clear
void AdventureUpdateProgress(AdventureState *adventure, Board *board,
                              int linesCleared, int combo, int points,
                              int diamondsCollected, int emeraldsCollected);

// Check if player has met all goals
bool AdventureCheckGoal(AdventureState *adventure);

// Check if player ran out of valid moves (level failed)
bool AdventureCheckFailure(AdventureState *adventure, Board *board, PieceSlot slots[3]);

// ─── Progress save/load ────────────────────────────────────────────────────
void AdventureLoadProgress(AdventureSaveData *save);
void AdventureSaveProgress(AdventureSaveData *save);

// Get the ice color that was underneath a cell (for rendering)
int AdventureGetIceUnderColor(AdventureState *adventure, int row, int col);

#endif // ADVENTURE_H