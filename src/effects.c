#include "effects.h"
#include "game.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


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

#ifndef PI
#define PI 3.14159265358979f
#endif

void ParticleEmit(ParticleSystem *ps, float x, float y,
                  Color color, int count)
{
    for (int i = 0; i < count; i++) {
        if (ps->count >= MAX_PARTICLES) return;

        Particle *p = &ps->items[ps->count++];
        p->active = true;
        p->pos = (Vector2){x, y};

        // Random direction and speed
        float angle = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
        float speed = 80.0f + ((float)rand() / (float)RAND_MAX) * 160.0f;
        p->vel = (Vector2){cosf(angle) * speed, sinf(angle) * speed};

        p->life = PARTICLE_LIFE + ((float)rand() / (float)RAND_MAX) * 0.4f;
        p->maxLife = p->life;
        p->size = 3.0f + ((float)rand() / (float)RAND_MAX) * 4.0f;
        p->gravity = 200.0f + ((float)rand() / (float)RAND_MAX) * 100.0f;

        // Slight color variation
        p->color = color;
        int variation = (rand() % 40) - 20;
        p->color.r = (unsigned char)fminf(255, fmaxf(0, color.r + variation));
        p->color.g = (unsigned char)fminf(255, fmaxf(0, color.g + variation));
    }
}

void ParticleUpdate(ParticleSystem *ps, float dt)
{
    bool anyActive = false;

    for (int i = 0; i < ps->count; i++) {
        Particle *p = &ps->items[i];
        if (!p->active) continue;

        p->life -= dt;
        if (p->life <= 0.0f) {
            p->active = false;
            continue;
        }

        anyActive = true;

        // Apply gravity
        p->vel.y += p->gravity * dt;

        // Move
        p->pos.x += p->vel.x * dt;
        p->pos.y += p->vel.y * dt;
    }

    // Reset count if all inactive
    if (!anyActive) ps->count = 0;
}

void ParticleDraw(ParticleSystem *ps)
{
    for (int i = 0; i < ps->count; i++) {
        Particle *p = &ps->items[i];
        if (!p->active) continue;

        float t = p->life / p->maxLife;  // 1.0 -> 0.0

        // Size shrinks over time
        float currentSize = p->size * t;
        if (currentSize < 0.5f) continue;

        // Alpha fades
        Color clr = p->color;
        clr.a = (unsigned char)(t * 255.0f);

        DrawRectangle(
            (int)(p->pos.x - currentSize / 2),
            (int)(p->pos.y - currentSize / 2),
            (int)currentSize + 1,
            (int)currentSize + 1,
            clr
        );
    }
}

void FloatTextAdd(FloatTextQueue *queue, const char *text,
                  float centerX, float startY,
                  int fontSize, Color color)
{
    // Find an inactive slot or use the oldest one
    int idx = -1;
    for (int i = 0; i < queue->count; i++) {
        if (!queue->items[i].active) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        if (queue->count < MAX_FLOAT_TEXTS) {
            idx = queue->count++;
        } else {
            // Overwrite the oldest (first) item
            idx = 0;
        }
    }

    FloatText *ft = &queue->items[idx];
    ft->active = true;
    strncpy(ft->text, text, FLOAT_TEXT_LEN - 1);
    ft->text[FLOAT_TEXT_LEN - 1] = '\0';
    ft->x = centerX;
    ft->y = startY;
    ft->startY = startY;
    ft->timer = FLOAT_TEXT_DURATION;
    ft->duration = FLOAT_TEXT_DURATION;
    ft->speed = FLOAT_TEXT_SPEED;
    ft->fontSize = fontSize;
    ft->color = color;
}

void FloatTextUpdate(FloatTextQueue *queue, float dt)
{
    for (int i = 0; i < queue->count; i++) {
        FloatText *ft = &queue->items[i];
        if (!ft->active) continue;

        ft->timer -= dt;
        ft->y -= ft->speed * dt;  // move upward

        if (ft->timer <= 0.0f) {
            ft->active = false;
        }
    }

    // Compact: if all inactive, reset count
    bool anyActive = false;
    for (int i = 0; i < queue->count; i++) {
        if (queue->items[i].active) {
            anyActive = true;
            break;
        }
    }
    if (!anyActive) queue->count = 0;
}

void FloatTextDraw(FloatTextQueue *queue)
{
    for (int i = 0; i < queue->count; i++) {
        FloatText *ft = &queue->items[i];
        if (!ft->active) continue;

        float t = ft->timer / ft->duration;  // 1.0 -> 0.0
        unsigned char alpha = (unsigned char)(t * 255.0f);

        // Scale effect: start slightly larger, settle to normal
        float scale = 1.0f + (1.0f - t) * 0.15f;
        int fontSize = (int)(ft->fontSize * scale);

        Color clr = ft->color;
        clr.a = alpha;

        int w = (int)MeasureTextEx(gameFont, ft->text, (float)fontSize, 1.0f).x;
        DrawTextEx(gameFont, ft->text, (Vector2){ft->x - w / 2.0f, ft->y},
                   (float)fontSize, 1.0f, clr);
    }
}
