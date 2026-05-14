#ifndef LEVEL_H
#define LEVEL_H

#include "defs.h"
#include "board.h"
#include "piece.h"
#include <stdbool.h>

// ─── Level Definition (immutable, const array) ────────────────────────────────
// Level 0 = classic (infinite play, all targets 0)
// Levels 1-10 = adventure levels
typedef struct {
    int level;
    int targetScore;      // 0 = no score target
    int targetDiamonds;   // 0 = no diamond target
    int targetEmeralds;   // 0 = no emerald target
    int prefillCount;     // random pieces to pre-place on board
} LevelDef;

// ─── Level State (mutable, runtime) ──────────────────────────────────────────
typedef struct {
    int currentLevel;         // index into level defs (0 = classic)
    int collectedDiamonds;
    int collectedEmeralds;
    bool levelComplete;
    bool levelFailed;
} LevelState;

// ─── Public Functions ─────────────────────────────────────────────────────────

// Get the level definitions array (index 0 = classic, 1-10 = adventure)
const LevelDef *LevelGetDefs(void);

// Load editable level definitions from data/levels.txt.
// Missing or invalid lines keep the built-in defaults.
void LevelLoadDefinitions(void);

// Initialize level state + board for a given level
void LevelInit(LevelState *state, int levelIndex, Board *board);

// Check if all goals are met. Returns false for classic (level 0).
bool LevelCheckGoal(LevelState *state, int currentScore);

// Check if no valid moves remain
bool LevelCheckFailure(Board *board, PieceSlot slots[3]);

static inline bool LevelIsUnlocked(int unlockedLevel, int lvl) {
    return (lvl >= 1 && lvl <= TOTAL_LEVELS && lvl <= unlockedLevel);
}

static inline bool LevelIsCompleted(int unlockedLevel, int lvl) {
    return (lvl >= 1 && lvl <= TOTAL_LEVELS && lvl < unlockedLevel);
}

#endif // LEVEL_H
