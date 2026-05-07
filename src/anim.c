#include "anim.h"

void AnimAddCleared(AnimQueue *queue, bool clearedCells[GRID_SIZE][GRID_SIZE])
{
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (!clearedCells[r][c]) continue;
            if (queue->count >= MAX_ANIMS * GRID_SIZE) return;

            ClearAnim *a = &queue->items[queue->count++];
            a->active = true;
            a->timer = CLEAR_ANIM_DURATION;
            a->row = r;
            a->col = c;
        }
    }
}

bool AnimUpdate(AnimQueue *queue, float dt)
{
    bool anyActive = false;

    for (int i = 0; i < queue->count; i++) {
        ClearAnim *a = &queue->items[i];
        if (!a->active) continue;

        a->timer -= dt;
        if (a->timer <= 0.0f) {
            a->active = false;
        } else {
            anyActive = true;
        }
    }

    // If no active animations, reset the queue
    if (!anyActive) {
        queue->count = 0;
    }

    return anyActive;
}

bool AnimIsActive(AnimQueue *queue)
{
    for (int i = 0; i < queue->count; i++) {
        if (queue->items[i].active) return true;
    }
    return false;
}

float AnimGetCellAlpha(AnimQueue *queue, int row, int col)
{
    for (int i = 0; i < queue->count; i++) {
        ClearAnim *a = &queue->items[i];
        if (a->active && a->row == row && a->col == col) {
            return a->timer / CLEAR_ANIM_DURATION; // 1.0 -> 0.0 fade out
        }
    }
    return -1.0f; // not being animated
}