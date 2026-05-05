#include "render.h"
#include "font.h"
#include "adventure.h"
#include <stdio.h>

// ----- Adventure Map -----
void RenderAdventureMap(GameState *state)
{
    // Title
    const char *mapTitle = "MACERA MODU";
    GameDrawText(mapTitle, (SCREEN_WIDTH - GameMeasureText(mapTitle, 36)) / 2, 30, 36, WHITE);

    // Lives display (hearts)
    char livesBuf[32];
    AdventureRegenLives(&state->adventureSave);
    int lives = state->adventureSave.currentLives;
    sprintf(livesBuf, "Can: ");
    int lw = GameMeasureText(livesBuf, 20);
    GameDrawText(livesBuf, 20, 80, 20, (Color){200, 100, 100, 255});

    // Draw hearts (using text symbols as before)
    for (int i = 0; i < MAX_LIVES; i++) {
        int hx = 20 + lw + i * 28;
        Color heartColor = (i < lives) ? (Color){255, 60, 60, 255} : (Color){60, 60, 80, 255};
        GameDrawText(i < lives ? "@" : "@", hx, 78, 22, heartColor);
    }

    // Total stars collected
    int totalStars = 0;
    int maxStars = TOTAL_LEVELS * 3;
    for (int i = 0; i < TOTAL_LEVELS; i++)
        totalStars += state->adventureSave.levels[i].bestStars;

    char starsBuf[64];
    sprintf(starsBuf, "Yildiz: %d / %d", totalStars, maxStars);
    int sw = GameMeasureText(starsBuf, 18);
    GameDrawText(starsBuf, SCREEN_WIDTH - sw - 20, 82, 18, (Color){255, 220, 50, 255});

    // No lives warning
    if (lives <= 0) {
        const char *noLivesMsg = "Caniniz kalmadi! 20 dk sonra yenilenir.";
        int nw = GameMeasureText(noLivesMsg, 16);
        GameDrawText(noLivesMsg, (SCREEN_WIDTH - nw) / 2, 115, 16, (Color){255, 100, 100, 255});
    }

    // Level grid
    int btnSize = 80;
    int gap = 15;
    int totalRowWidth = LEVELS_PER_ROW * btnSize + (LEVELS_PER_ROW - 1) * gap;
    int mapStartX = (SCREEN_WIDTH - totalRowWidth) / 2;
    int mapStartY = 150;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        int row = i / LEVELS_PER_ROW;
        int col = i % LEVELS_PER_ROW;
        int bx = mapStartX + col * (btnSize + gap);
        int by = mapStartY + row * (btnSize + gap + 30);
        Rectangle btnRect = { bx, by, btnSize, btnSize };

        bool isUnlocked = state->adventureSave.levels[i].unlocked;
        bool hover = CheckCollisionPointRec(mouse, btnRect) && isUnlocked;
        int stars = state->adventureSave.levels[i].bestStars;

        // Button background
        Color btnColor;
        if (!isUnlocked) {
            btnColor = (Color){25, 25, 40, 255};
        } else if (hover) {
            btnColor = (Color){60, 50, 80, 255};
        } else {
            btnColor = (Color){40, 35, 60, 255};
        }
        Color borderColor = isUnlocked ? (Color){120, 80, 180, 255} : (Color){40, 40, 55, 255};

        DrawRectangleRounded(btnRect, 0.2f, 6, btnColor);
        DrawRectangleRoundedLines(btnRect, 0.2f, 6, borderColor);

        // Level number
        char lvlBuf[8];
        sprintf(lvlBuf, "%d", i + 1);
        int lnw = GameMeasureText(lvlBuf, 28);
        Color lvlColor = isUnlocked ? WHITE : (Color){60, 60, 80, 255};
        GameDrawText(lvlBuf, bx + (btnSize - lnw) / 2, by + 20, 28, lvlColor);

        // Lock icon for locked levels
        if (!isUnlocked) {
            GameDrawText("K", bx + btnSize / 2 - 8, by + btnSize - 22, 18, (Color){60, 60, 80, 255});
        }

        // Stars below each button
        int starOffsetX = bx + (btnSize - 3 * 16) / 2;
        for (int s = 0; s < 3; s++) {
            Color starColor;
            if (!isUnlocked) {
                starColor = (Color){30, 30, 45, 255};
            } else if (s < stars) {
                starColor = (Color){255, 220, 50, 255};
            } else {
                starColor = (Color){60, 60, 80, 100};
            }
            GameDrawText("*", starOffsetX + s * 16, by + btnSize + 5, 16, starColor);
        }
    }

    // Hint at bottom
    const char *escHint = "ESC: Ana Menu";
    GameDrawText(escHint, (SCREEN_WIDTH - GameMeasureText(escHint, 16)) / 2,
             SCREEN_HEIGHT - 30, 16, (Color){100, 100, 120, 255});
}

// ----- Adventure Play HUD -----
void RenderAdventurePlay(GameState *state)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[state->adventure.currentLevel];

    char buf[128];

    // Level info at top
    int fontSize = 22;
    sprintf(buf, "Level %d", state->adventure.currentLevel + 1);
    int lw = GameMeasureText(buf, fontSize);
    GameDrawText(buf, 10, 10, fontSize, (Color){180, 150, 255, 255});

    // Lives next to level
    int lives = state->adventureSave.currentLives;
    for (int i = 0; i < MAX_LIVES; i++) {
        int hx = 10 + lw + 10 + i * 22;
        GameDrawText(i < lives ? "@" : "@", hx, 10, 18,
            i < lives ? (Color){255, 60, 60, 255} : (Color){60, 60, 80, 255});
    }

    // Goal description
    sprintf(buf, "Hedef: %s", level->description);
    int gw = GameMeasureText(buf, 16);
    GameDrawText(buf, SCREEN_WIDTH - gw - 10, 12, 16, (Color){200, 200, 220, 255});

    // Progress bar
    int barY = 40;
    int barWidth = SCREEN_WIDTH - 80;
    int barHeight = 14;
    int barX = 40;

    // Calculate progress percentage
    float progress = 0.0f;
    if (level->goalValue2 > 0) {
        float p1 = (float)state->adventure.goalProgress / (float)level->goalValue;
        float p2 = (float)state->adventure.goalProgress2 / (float)level->goalValue2;
        progress = (p1 + p2) / 2.0f;
    } else {
        progress = (float)state->adventure.goalProgress / (float)level->goalValue;
    }
    if (progress > 1.0f) progress = 1.0f;

    // Bar background
    DrawRectangle(barX, barY, barWidth, barHeight, (Color){40, 40, 60, 255});
    DrawRectangleLines(barX, barY, barWidth, barHeight, (Color){60, 60, 80, 255});

    // Bar fill (gradient effect)
    Color barColor;
    if (progress < 0.5f)
        barColor = (Color){255, 100, 100, 255};
    else if (progress < 0.8f)
        barColor = (Color){255, 200, 50, 255};
    else
        barColor = (Color){50, 255, 100, 255};

    DrawRectangle(barX + 1, barY + 1,
                  (int)((barWidth - 2) * progress), barHeight - 2, barColor);

    // Progress text
    if (level->goalValue2 > 0) {
        sprintf(buf, "%d/%d  %d/%d",
                state->adventure.goalProgress, level->goalValue,
                state->adventure.goalProgress2, level->goalValue2);
    } else {
        sprintf(buf, "%d / %d", state->adventure.goalProgress, level->goalValue);
    }
    int pw = GameMeasureText(buf, 14);
    GameDrawText(buf, (SCREEN_WIDTH - pw) / 2, barY + 1, 14, (Color){220, 220, 240, 255});

    // Pieces remaining
    int piecesRemaining = level->pieceLimit - state->adventure.piecesUsed;
    if (piecesRemaining < 0) piecesRemaining = 0;
    sprintf(buf, "Kalan parca: %d", piecesRemaining);
    GameDrawText(buf, 10, barY + barHeight + 5, 16, (Color){180, 180, 200, 255});

    // Score (for GOAL_SCORE type)
    if (level->goalType == GOAL_SCORE || level->goalType == GOAL_MIXED) {
        sprintf(buf, "Puan: %d", state->score);
        int sw = GameMeasureText(buf, 16);
        GameDrawText(buf, SCREEN_WIDTH - sw - 10, barY + barHeight + 5, 16, (Color){180, 180, 200, 255});
    }

    // Render board
    RenderBoard(state);

    // Render piece slots
    RenderPieceSlots(state);
}

// ----- Adventure Result Screen -----
void RenderAdventureResult(GameState *state)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    if (state->adventure.levelFailed) {
        const char *failMsg = "SEVIYE BASARISIZ!";
        int fw = GameMeasureText(failMsg, 36);
        GameDrawText(failMsg, (SCREEN_WIDTH - fw) / 2, 280, 36, (Color){255, 80, 80, 255});

        const char *subMsg = "Bir can kaybettiniz!";
        int sw = GameMeasureText(subMsg, 20);
        GameDrawText(subMsg, (SCREEN_WIDTH - sw) / 2, 340, 20, (Color){200, 200, 220, 255});
    } else {
        const char *successMsg = "SEVIYE TAMAMLANDI!";
        int sw = GameMeasureText(successMsg, 36);
        GameDrawText(successMsg, (SCREEN_WIDTH - sw) / 2, 270, 36, (Color){50, 255, 100, 255});

        // Stars earned
        int stars = state->adventure.earnedStars;
        int starX = (SCREEN_WIDTH - stars * 36) / 2;
        for (int i = 0; i < stars; i++) {
            GameDrawText("*", starX + i * 36, 320, 32, (Color){255, 220, 50, 255});
        }
    }
}