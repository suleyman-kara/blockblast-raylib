#include "render.h"
#include "defs.h"
#include "textures.h"
#include <stdio.h>
#include <math.h>

// ----- Gear icon (settings button, top-right) -----
void RenderGearIcon(void)
{
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