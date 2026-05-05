#ifndef ADVENTURE_H
#define ADVENTURE_H

#include "core/config.h"
#include "core/board.h"
#include "core/piece.h"
#include <stdbool.h>
#include <time.h>

// ─── Cell types for obstacles ─────────────────────────────────────────────────
// (CELL_ICE and CELL_STONE are already defined in config.h)

// ─── Level data ───────────────────────────────────────────────────────────────
typedef enum {
    GOAL_CLEAR_LINES,   // Clear N lines (rows/columns)
    GOAL_SCORE,         // Reach N points
    GOAL_COMBO,         // Perform N combos
    GOAL_MIXED,         // Multiple goals combined
} GoalType;

typedef struct {
    int row;
    int col;
} ObstaclePos;

typedef struct {
    int levelNumber;
    GoalType goalType;
    int goalValue;       // Target (e.g., 5 lines, 1000 points, 3 combos)
    GoalType goalType2;  // For mixed goals (GOAL_MIXED)
    int goalValue2;
    int pieceLimit;      // Max pieces allowed
    int obstacleCount;
    ObstaclePos obstacles[MAX_OBSTACLES];
    bool isIce[MAX_OBSTACLES];  // true = ice, false = stone
    const char *description;    // e.g., "5 lines clear"
} LevelDef;

// ─── Progress data (saved to file) ──────────────────────────────────────────
typedef struct {
    bool unlocked;
    int bestStars;     // 0-3
    bool completed;
} LevelProgress;

typedef struct {
    LevelProgress levels[TOTAL_LEVELS];
    int currentLives;
    time_t lastLifeRegen;  // timestamp of last life regen
} AdventureSaveData;

// ─── Adventure state (runtime) ──────────────────────────────────────────────
typedef struct {
    int currentLevel;       // 0-based index into levelDefs
    int piecesUsed;         // how many pieces placed
    int goalProgress;       // current progress toward goal (e.g., lines cleared)
    int goalProgress2;      // second goal progress (for mixed)
    bool goalReached;
    bool levelFailed;
    int earnedStars;
    bool levelComplete;
    bool showResultScreen;
    float resultTimer;

    // Combo tracking
    int comboCount;         // number of combos performed in this level

    // Board obstacle data
    bool boardHasObstacles;
    int iceUnderColor[GRID_SIZE][GRID_SIZE];  // color hidden under ice
} AdventureState;

// ─── Public functions ──────────────────────────────────────────────────────

// Get the level definitions array
const LevelDef *AdventureGetLevelDefs(void);

// Initialize adventure state for a given level (loads board obstacles)
void AdventureInitLevel(AdventureState *adventure, int levelIndex, Board *board);

// Check if a cell is occupied by an obstacle
bool AdventureIsCellBlocked(AdventureState *adventure, int row, int col);

// Update goal progress after a piece placement + line clear
void AdventureUpdateProgress(AdventureState *adventure, Board *board,
                              int linesCleared, int combo, int points);

// Check if player has met the goal
bool AdventureCheckGoal(AdventureState *adventure);

// Calculate star rating based on progress vs goal
int AdventureCalcStars(AdventureState *adventure);

// Check if player ran out of pieces (level failed)
bool AdventureCheckFailure(AdventureState *adventure);

// ─── Life system ───────────────────────────────────────────────────────────

void AdventureLoadLives(AdventureSaveData *save);
void AdventureSaveLives(AdventureSaveData *save);
void AdventureRegenLives(AdventureSaveData *save);

// ─── Progress save/load ────────────────────────────────────────────────────

void AdventureLoadProgress(AdventureSaveData *save);
void AdventureSaveProgress(AdventureSaveData *save);

// Get the ice color that was underneath a cell (for rendering)
int AdventureGetIceUnderColor(AdventureState *adventure, int row, int col);

#endif // ADVENTURE_H