#ifndef ANIM_H
#define ANIM_H

#include "raylib.h"
#include "core/defs.h"
#include <stdbool.h>

// A single cell-clear animation
typedef struct {
    bool active;
    float timer;      // remaining time (seconds)
    int row;
    int col;
} ClearAnim;

// Queue of active animations
typedef struct {
    ClearAnim items[MAX_ANIMS * GRID_SIZE]; // worst case: 16 lines * 8 cells
    int count;
} AnimQueue;

// Add animations for all cleared cells (marked true in clearedCells)
void AnimAddCleared(AnimQueue *queue, bool clearedCells[GRID_SIZE][GRID_SIZE]);

// Update all active animations by dt. Returns true if any animation is still active.
bool AnimUpdate(AnimQueue *queue, float dt);

// Check if any animation is currently playing
bool AnimIsActive(AnimQueue *queue);

// Get the alpha (0.0-1.0) for a cell if it's being animated, or -1 if not
float AnimGetCellAlpha(AnimQueue *queue, int row, int col);

#endif // ANIM_H