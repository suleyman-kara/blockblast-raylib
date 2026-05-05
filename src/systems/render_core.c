#include "render.h"
#include "font.h"
#include "theme.h"
#include <stdio.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

// ----- Score display -----
void RenderScore(GameState *state)
{
    char buf[64];
    const TextStyle *txt = &THEME_DEFAULT.text;

    // Score
    sprintf(buf, "SCORE: %d", state->score);
    int w = GameMeasureText(buf, 28);
    GameDrawText(buf, (SCREEN_WIDTH - w) / 2, 20, 28, txt->primary);

    // High score
    sprintf(buf, "BEST: %d", state->highScore);
    w = GameMeasureText(buf, 18);
    GameDrawText(buf, (SCREEN_WIDTH - w) / 2, 55, 18, txt->secondary);

    // Combo
    if (state->combo > 1) {
        sprintf(buf, "COMBO x%d", state->combo);
        w = GameMeasureText(buf, 22);
        GameDrawText(buf, (SCREEN_WIDTH - w) / 2, 85, 22,
            (Color){255, 220, 50, 255});
    }
}

// ----- Best Combo banner -----
void RenderBanner(GameState *state)
{
    if (!state->banner.active) return;

    float t = state->banner.timer / state->banner.duration; // 1.0 -> 0.0
    float progress = 1.0f - t; // 0.0 -> 1.0

    // Pulse effect on font size
    int baseSize = THEME_DEFAULT.effects.bannerPulseBaseSize;
    int pulseSize = baseSize + (int)(8.0f * sinf(progress * PI * 4.0f));

    // Alpha: fade in quickly, stay, fade out at the end
    float alpha;
    if (progress < 0.15f) {
        alpha = progress / 0.15f;          // fade in
    } else if (progress > 0.7f) {
        alpha = (1.0f - progress) / 0.3f;  // fade out
    } else {
        alpha = 1.0f;                       // fully visible
    }

    Color clr = THEME_DEFAULT.effects.bannerText;
    clr.a = (unsigned char)(alpha * 255.0f);
    Color shadow = THEME_DEFAULT.effects.bannerShadow;
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

// ----- Draw a gem icon inside a cell block -----
void DrawGemIcon(int x, int y, int cellSize, int gemType)
{
    const GemStyle *g = &THEME_DEFAULT.gem;
    int gemSize = (int)(cellSize * g->sizeRatio);
    int half = gemSize / 2;
    int cx = x + cellSize / 2;
    int cy = y + cellSize / 2;

    if (gemType == GEM_DIAMOND) {
        // Diamond: cyan/white triangle (pointing down)
        DrawTriangle((Vector2){cx, cy + half},
                     (Vector2){cx - half, cy},
                     (Vector2){cx, cy - half},
                     g->diamondPrimary);
        DrawTriangle((Vector2){cx, cy + half},
                     (Vector2){cx + half, cy},
                     (Vector2){cx, cy - half},
                     g->diamondHighlight);
    } else if (gemType == GEM_EMERALD) {
        // Emerald: green triangle (pointing down)
        DrawTriangle((Vector2){cx, cy + half},
                     (Vector2){cx - half, cy},
                     (Vector2){cx, cy - half},
                     g->emeraldPrimary);
        DrawTriangle((Vector2){cx, cy + half},
                     (Vector2){cx + half, cy},
                     (Vector2){cx, cy - half},
                     g->emeraldHighlight);
    }
}