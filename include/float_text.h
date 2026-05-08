#ifndef FLOAT_TEXT_H
#define FLOAT_TEXT_H

#include "raylib.h"
#include "defs.h"
#include <stdbool.h>

// A single floating text element (e.g. "+300")
typedef struct {
    bool active;
    char text[FLOAT_TEXT_LEN];
    float x, y;           // current position
    float startY;         // initial Y (for alpha calculation)
    float timer;           // remaining time
    float duration;        // total duration
    float speed;           // upward movement speed (px/s)
    int fontSize;
    Color color;
} FloatText;

// Queue of active floating texts
typedef struct {
    FloatText items[MAX_FLOAT_TEXTS];
    int count;
} FloatTextQueue;

// Add a floating text centered at (centerX, startY)
void FloatTextAdd(FloatTextQueue *queue, const char *text,
                  float centerX, float startY,
                  int fontSize, Color color);

// Update all active float texts
void FloatTextUpdate(FloatTextQueue *queue, float dt);

// Draw all active float texts
void FloatTextDraw(FloatTextQueue *queue);

#endif // FLOAT_TEXT_H