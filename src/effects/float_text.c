#include "float_text.h"
#include "game.h"
#include <stdio.h>
#include <string.h>

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