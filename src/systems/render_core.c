#include "render.h"
#include "font.h"
#include "defs.h"
#include "textures.h"
#include <stdio.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

// ----- Score display (Classic Play HUD) -----
void RenderScore(GameState *state)
{
    // Top Left: Crown icon + Top Score (Yellow, no text label)
    int crownSize = 28;
    DrawTexturePro(gameTextures.crown,
        (Rectangle){ 0, 0, (float)gameTextures.crown.width, (float)gameTextures.crown.height },
        (Rectangle){ 10, 12, (float)crownSize, (float)crownSize },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    char bestBuf[16];
    sprintf(bestBuf, "%d", state->highScore);
    GameDrawText(bestBuf, 10 + crownSize + 6, 14, 20, (Color){255, 220, 50, 255}); // Yellow

    // Top Middle: Score (big, just the number, no text)
    char scoreBuf[16];
    sprintf(scoreBuf, "%d", state->score);
    int scoreW = GameMeasureText(scoreBuf, 36);
    GameDrawText(scoreBuf, (SCREEN_WIDTH - scoreW) / 2, 8, 36, COLOR_TEXT_PRIMARY);

    // Combo
    if (state->combo > 1) {
        char comboBuf[16];
        sprintf(comboBuf, "COMBO x%d", state->combo);
        int cw = GameMeasureText(comboBuf, 18);
        GameDrawText(comboBuf, (SCREEN_WIDTH - cw) / 2, 50, 18,
            (Color){255, 220, 50, 255});
    }

    // Top Right: Gear icon is drawn separately by RenderGearIcon()
}

// ----- Best Combo banner -----
void RenderBanner(GameState *state)
{
    if (!state->banner.active) return;

    float t = state->banner.timer / state->banner.duration; // 1.0 -> 0.0
    float progress = 1.0f - t; // 0.0 -> 1.0

    // Pulse effect on font size
    int pulseSize = BANNER_PULSE_BASE_SIZE + (int)(8.0f * sinf(progress * PI * 4.0f));

    // Alpha: fade in quickly, stay, fade out at the end
    float alpha;
    if (progress < 0.15f) {
        alpha = progress / 0.15f;          // fade in
    } else if (progress > 0.7f) {
        alpha = (1.0f - progress) / 0.3f;  // fade out
    } else {
        alpha = 1.0f;                       // fully visible
    }

    Color clr = COLOR_BANNER_TEXT;
    clr.a = (unsigned char)(alpha * 255.0f);
    Color shadow = COLOR_BANNER_SHADOW;
    shadow.a = (unsigned char)(alpha * 120.0f);

    int w = GameMeasureText(state->banner.text, pulseSize);
    int x = (SCREEN_WIDTH - w) / 2;
    int y = 100;

    // Drop shadow
    GameDrawText(state->banner.text, x + 2, y + 2, pulseSize, shadow);
    // Main text
    GameDrawText(state->banner.text, x, y, pulseSize, clr);
}

// ----- Gear icon (settings button, top-right) -----
void RenderGearIcon(void)
{
    // Draw a simple gear/cog icon using basic shapes
    int cx = GEAR_X + GEAR_SIZE / 2;
    int cy = GEAR_Y + GEAR_SIZE / 2;
    int outerR = GEAR_SIZE / 2 - 2;
    int innerR = outerR - 7;
    int holeR  = 5;

    Color gearClr = (Color){180, 180, 200, 200};
    Vector2 mousePos = GetMousePosition();
    Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
    if (CheckCollisionPointRec(mousePos, gearRect)) {
        gearClr = (Color){255, 220, 50, 240}; // highlight on hover
    }

    // Outer circle
    DrawCircle(cx, cy, outerR, gearClr);

    // Gear teeth (8 small rectangles around the circle)
    for (int i = 0; i < 8; i++) {
        float angle = (float)i * (PI / 4.0f);
        float tx = cx + cosf(angle) * (outerR - 1);
        float ty = cy + sinf(angle) * (outerR - 1);
        DrawRectanglePro(
            (Rectangle){ tx, ty, 8, 5 },
            (Vector2){ 4, 2.5f },
            angle * (180.0f / PI),
            gearClr
        );
    }

    // Inner circle (darker, to create the ring look)
    DrawCircle(cx, cy, innerR, (Color){30, 30, 48, 255});

    // Center hole
    DrawCircle(cx, cy, holeR, gearClr);
    DrawCircle(cx, cy, holeR - 2, (Color){30, 30, 48, 255});
}

// ----- Draw a gem icon inside a cell block (using PNG textures) -----
void DrawGemIcon(int x, int y, int cellSize, int gemType)
{
    int gemSize = (int)(cellSize * GEM_SIZE_RATIO);
    int cx = x + cellSize / 2;
    int cy = y + cellSize / 2;

    if (gemType == GEM_DIAMOND) {
        DrawTexturePro(gameTextures.diamond,
            (Rectangle){ 0, 0, (float)gameTextures.diamond.width, (float)gameTextures.diamond.height },
            (Rectangle){ (float)(cx - gemSize/2), (float)(cy - gemSize/2), (float)gemSize, (float)gemSize },
            (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else if (gemType == GEM_EMERALD) {
        DrawTexturePro(gameTextures.emerald,
            (Rectangle){ 0, 0, (float)gameTextures.emerald.width, (float)gameTextures.emerald.height },
            (Rectangle){ (float)(cx - gemSize/2), (float)(cy - gemSize/2), (float)gemSize, (float)gemSize },
            (Vector2){ 0, 0 }, 0.0f, WHITE);
    }
}