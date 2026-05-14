#ifndef EFFECTS_H
#define EFFECTS_H

#include "raylib.h"
#include "defs.h"
#include <stdbool.h>

typedef struct {
    bool active;
    float timer;
    int row;
    int col;
} ClearAnim;

typedef struct {
    ClearAnim items[MAX_ANIMS * GRID_SIZE];
    int count;
} AnimQueue;

typedef struct {
    bool active;
    Vector2 pos;
    Vector2 vel;
    float life;
    float maxLife;
    float size;
    Color color;
    float gravity;
} Particle;

typedef struct {
    Particle items[MAX_PARTICLES];
    int count;
} ParticleSystem;

typedef struct {
    bool active;
    char text[FLOAT_TEXT_LEN];
    float x, y;
    float startY;
    float timer;
    float duration;
    float speed;
    int fontSize;
    Color color;
} FloatText;

typedef struct {
    FloatText items[MAX_FLOAT_TEXTS];
    int count;
} FloatTextQueue;

void AnimAddCleared(AnimQueue *queue, bool clearedCells[GRID_SIZE][GRID_SIZE]);
bool AnimUpdate(AnimQueue *queue, float dt);
bool AnimIsActive(AnimQueue *queue);
float AnimGetCellAlpha(AnimQueue *queue, int row, int col);

void ParticleEmit(ParticleSystem *ps, float x, float y, Color color, int count);
void ParticleUpdate(ParticleSystem *ps, float dt);
void ParticleDraw(ParticleSystem *ps);

void FloatTextAdd(FloatTextQueue *queue, const char *text,
                  float centerX, float startY,
                  int fontSize, Color color);
void FloatTextUpdate(FloatTextQueue *queue, float dt);
void FloatTextDraw(FloatTextQueue *queue);

#endif // EFFECTS_H
