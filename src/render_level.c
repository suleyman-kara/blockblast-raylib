#include "render.h"
#include "level.h"
#include "defs.h"
#include "textures.h"
#include <stdio.h>

// ─── Adventure Map UI Constants ───────────────────────────────────────────────
#define COLOR_AMAP_LOCKED_BG       ((Color){ 25,  25,  40,  255 })
#define COLOR_AMAP_LOCKED_BORDER   ((Color){ 40,  40,  55,  255 })
#define COLOR_AMAP_UNLOCKED_BG     ((Color){ 40,  35,  60,  255 })
#define COLOR_AMAP_UNLOCKED_HOVER  ((Color){ 60,  50,  80,  255 })
#define COLOR_AMAP_UNLOCKED_BORDER ((Color){ 120, 80,  180, 255 })
#define COLOR_AMAP_COMPLETED_TEXT  ((Color){ 50,  255, 100, 255 })
#define COLOR_AMAP_LOCKED_NUMBER   ((Color){ 60,  60,  80,  255 })
#define AMAP_BTN_SIZE              80
#define AMAP_BTN_GAP               15
#define AMAP_BTN_LABEL_GAP         30
#define AMAP_START_Y               150

// ─── Helper: draw texture full ───────────────────────────────────────────────
static void DrawTextureFull(Texture2D tex, int x, int y, int w, int h)
{
    DrawTexturePro(tex,
        (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
        (Rectangle){ (float)x, (float)y, (float)w, (float)h },
        (Vector2){ 0, 0 }, 0.0f, WHITE);
}

// ─── Helper: draw text centered X ────────────────────────────────────────────
static void DrawTextCenteredX(const char *text, int y, int fontSize, Color color)
{
    int tw = (int)MeasureTextEx(gameFont, text, (float)fontSize, 1.0f).x;
    DrawTextEx(gameFont, text, (Vector2){(SCREEN_WIDTH - tw) / 2.0f, (float)y},
               (float)fontSize, 1.0f, color);
}

// ─── Helper: draw styled button ──────────────────────────────────────────────
static void DrawButtonStyled(Rectangle btn, const char *text, int fontSize,
                              Color bg, Color bgHover,
                              Color border, Color borderHover,
                              Color textColor, Color textHover,
                              bool hover)
{
    Color cBg  = hover ? bgHover  : bg;
    Color cBrd = hover ? borderHover : border;
    Color cTxt = hover ? textHover : textColor;

    DrawRectangleRounded(btn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, cBg);
    DrawRectangleRoundedLines(btn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, cBrd);

    int tw = (int)MeasureTextEx(gameFont, text, (float)fontSize, 1.0f).x;
    DrawTextEx(gameFont, text,
               (Vector2){btn.x + (btn.width  - (float)tw)  / 2.0f,
                         btn.y + (btn.height - (float)fontSize) / 2.0f},
               (float)fontSize, 1.0f, cTxt);
}

// ─── Helper: draw gem counter ────────────────────────────────────────────────
static void DrawGemCounter(Texture2D tex, int centerX, int topY, int gemSize,
                            const char *countText, int fontSize, int textYOffset,
                            Color textColor)
{
    DrawTextureFull(tex, centerX - gemSize / 2, topY, gemSize, gemSize);
    int tw = (int)MeasureTextEx(gameFont, countText, (float)fontSize, 1.0f).x;
    DrawTextEx(gameFont, countText,
               (Vector2){(float)(centerX - tw / 2), (float)(topY + textYOffset)},
               (float)fontSize, 1.0f, textColor);
}

// ============================================================================
//  Play HUD — adapts based on level targets
// ============================================================================
void RenderPlayHUD(GameState *state)
{
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
    char buf[128];

    if (state->selectedLevel == 0) {
        // Classic mode: crown + high score + score + combo
        int crownSize = 28;
        DrawTexturePro(gameTextures.crown,
            (Rectangle){ 0, 0, (float)gameTextures.crown.width, (float)gameTextures.crown.height },
            (Rectangle){ 10, 12, (float)crownSize, (float)crownSize },
            (Vector2){ 0, 0 }, 0.0f, WHITE);

        sprintf(buf, "%d", state->highScore);
        DrawTextEx(gameFont, buf, (Vector2){10 + crownSize + 6, 14}, 20.0f, 1.0f, (Color){255, 220, 50, 255});

        sprintf(buf, "%d", state->score);
        int scoreW = (int)MeasureTextEx(gameFont, buf, 36.0f, 1.0f).x;
        DrawTextEx(gameFont, buf, (Vector2){(SCREEN_WIDTH - scoreW) / 2.0f, 8}, 36.0f, 1.0f, COLOR_TEXT_PRIMARY);

        if (state->combo > 1) {
            sprintf(buf, "COMBO x%d", state->combo);
            int cw = (int)MeasureTextEx(gameFont, buf, 18.0f, 1.0f).x;
            DrawTextEx(gameFont, buf, (Vector2){(SCREEN_WIDTH - cw) / 2.0f, 50}, 18.0f, 1.0f, (Color){255, 220, 50, 255});
        }
    } else {
        // Adventure mode: show only non-zero targets
        int midY = 12;
        bool hasScore = (def->targetScore > 0);
        bool hasDiamond = (def->targetDiamonds > 0);
        bool hasEmerald = (def->targetEmeralds > 0);

        if (hasScore && !hasDiamond && !hasEmerald) {
            // Score only
            sprintf(buf, "%d / %d", state->score, def->targetScore);
            DrawTextCenteredX(buf, midY, 36, COLOR_TEXT_PRIMARY);
        } else if (!hasScore && hasDiamond && !hasEmerald) {
            // Diamond only
            int gemSize = 32;
            int remaining = def->targetDiamonds - state->level.collectedDiamonds;
            if (remaining < 0) remaining = 0;
            sprintf(buf, "%d", remaining);
            DrawGemCounter(gameTextures.diamond, SCREEN_WIDTH / 2, midY,
                           gemSize, buf, 22, gemSize + 4, COLOR_TEXT_PRIMARY);
        } else {
            // Mixed: show all non-zero targets
            int gemSize = 24;
            int spacing = 80;
            int centerX = SCREEN_WIDTH / 2;

            if (hasScore) {
                sprintf(buf, "%d / %d", state->score, def->targetScore);
                DrawTextCenteredX(buf, midY + 4, 28, COLOR_TEXT_PRIMARY);
            }

            if (hasDiamond) {
                int remaining = def->targetDiamonds - state->level.collectedDiamonds;
                if (remaining < 0) remaining = 0;
                sprintf(buf, "%d", remaining);
                DrawGemCounter(gameTextures.diamond, centerX - spacing, midY + 2,
                               gemSize, buf, 16, gemSize + 6, COLOR_TEXT_PRIMARY);
            }

            if (hasEmerald) {
                int remaining = def->targetEmeralds - state->level.collectedEmeralds;
                if (remaining < 0) remaining = 0;
                sprintf(buf, "%d", remaining);
                DrawGemCounter(gameTextures.emerald, centerX + spacing, midY + 2,
                               gemSize, buf, 16, gemSize + 6, COLOR_TEXT_PRIMARY);
            }
        }
    }
}

// ============================================================================
//  Level Select Screen
// ============================================================================
void RenderLevelSelect(GameState *state)
{
    DrawTextureFull(gameTextures.logout, 15, 15, 32, 32);
    DrawTextCenteredX("ADVENTURE", 30, 36, COLOR_TEXT_PRIMARY);

    int totalRowWidth = LEVELS_PER_ROW * AMAP_BTN_SIZE + (LEVELS_PER_ROW - 1) * AMAP_BTN_GAP;
    int mapStartX = (SCREEN_WIDTH - totalRowWidth) / 2;
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        int row = i / LEVELS_PER_ROW;
        int col = i % LEVELS_PER_ROW;
        int bx = mapStartX + col * (AMAP_BTN_SIZE + AMAP_BTN_GAP);
        int by = AMAP_START_Y + row * (AMAP_BTN_SIZE + AMAP_BTN_GAP + AMAP_BTN_LABEL_GAP);
        Rectangle btnRect = { bx, by, AMAP_BTN_SIZE, AMAP_BTN_SIZE };

        bool isUnlocked = LevelIsUnlocked(state->levelCompleted, i + 1);
        bool isCompleted = state->levelCompleted[i];
        bool hover = CheckCollisionPointRec(mouse, btnRect) && isUnlocked;

        Color btnColor = !isUnlocked ? COLOR_AMAP_LOCKED_BG :
                         hover ? COLOR_AMAP_UNLOCKED_HOVER : COLOR_AMAP_UNLOCKED_BG;
        Color borderColor = !isUnlocked ? COLOR_AMAP_LOCKED_BORDER : COLOR_AMAP_UNLOCKED_BORDER;

        DrawRectangleRounded(btnRect, 0.2f, 6, btnColor);
        DrawRectangleRoundedLines(btnRect, 0.2f, 6, borderColor);

        char lvlBuf[8];
        sprintf(lvlBuf, "%d", i + 1);
        Color lvlColor = isUnlocked ? COLOR_TEXT_PRIMARY : COLOR_AMAP_LOCKED_NUMBER;
        int lnw = (int)MeasureTextEx(gameFont, lvlBuf, 28.0f, 1.0f).x;
        DrawTextEx(gameFont, lvlBuf,
                   (Vector2){(float)(bx + (AMAP_BTN_SIZE - lnw) / 2), (float)(by + 20)},
                   28.0f, 1.0f, lvlColor);

        if (isCompleted) {
            int compSize = 28;
            DrawTextureFull(gameTextures.completed,
                bx + AMAP_BTN_SIZE - compSize + 6,
                by - 6,
                compSize, compSize);
        } else if (!isUnlocked) {
            int lockSize = 24;
            DrawTextureFull(gameTextures.lock,
                bx + (AMAP_BTN_SIZE - lockSize) / 2,
                by + AMAP_BTN_SIZE - lockSize - 8,
                lockSize, lockSize);
        }
    }

    DrawTextCenteredX("ESC: Main Menu", SCREEN_HEIGHT - 30, 16, COLOR_TEXT_MUTED);
}

// ============================================================================
//  Result Screen (adaptive win/lose)
// ============================================================================
void RenderResult(GameState *state)
{
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];

    // Draw game board underneath
    RenderPlayHUD(state);
    RenderBoard(state);
    RenderPieceSlots(state);

    // Dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    // Card
    int cardW = 380, cardH = 400;
    int cardX = (SCREEN_WIDTH - cardW) / 2;
    int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
    DrawRectangleRounded((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){20, 25, 50, 235});
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){60, 70, 110, 180});

    char buf[128];
    Vector2 mouse = GetMousePosition();

    // Title
    if (state->level.levelFailed) {
        if (state->selectedLevel == 0) {
            DrawTextCenteredX("GAME OVER", cardY + 30, 32, (Color){255, 80, 80, 255});
        } else {
            DrawTextCenteredX("LEVEL FAILED!", cardY + 30, 32, (Color){255, 80, 80, 255});
        }
    } else {
        DrawTextCenteredX("LEVEL COMPLETE!", cardY + 30, 32, (Color){50, 255, 100, 255});
    }

    // Score
    int statY = cardY + 100;
    sprintf(buf, "Score: %d", state->score);
    DrawTextCenteredX(buf, statY, 28, WHITE);

    if (state->selectedLevel == 0) {
        // Classic: show high score
        sprintf(buf, "Best: %d", state->highScore);
        DrawTextCenteredX(buf, statY + 40, 22, (Color){150, 150, 170, 255});
    } else {
        // Adventure: show gem progress
        int infoY = statY + 45;
        if (def->targetDiamonds > 0) {
            sprintf(buf, "Diamonds: %d / %d", state->level.collectedDiamonds, def->targetDiamonds);
            DrawTextCenteredX(buf, infoY, 18, (Color){100, 200, 255, 255});
            infoY += 30;
        }
        if (def->targetEmeralds > 0) {
            sprintf(buf, "Emeralds: %d / %d", state->level.collectedEmeralds, def->targetEmeralds);
            DrawTextCenteredX(buf, infoY, 18, (Color){50, 220, 100, 255});
        }
    }

    // Buttons
    const int topBtnY = cardY + cardH - 140;
    const int botBtnY = topBtnY + BTN_H + BTN_GAP;
    Rectangle btnTop = { BTN_X, topBtnY, BTN_W, BTN_H };
    Rectangle btnBot = { BTN_X, botBtnY, BTN_W, BTN_H };
    bool topHover = CheckCollisionPointRec(mouse, btnTop);
    bool botHover = CheckCollisionPointRec(mouse, btnBot);

    if (state->level.levelFailed) {
        const char *retryText = (state->selectedLevel == 0) ? "Play Again" : "Try Again";
        DrawButtonStyled(btnTop, retryText, 18,
            (Color){55, 40, 85, 255}, (Color){80, 50, 120, 255},
            (Color){100, 70, 150, 255}, (Color){180, 120, 255, 255},
            (Color){170, 150, 200, 255}, (Color){255, 255, 255, 255},
            topHover);
    } else {
        const char *topText = (state->level.currentLevel < TOTAL_LEVELS) ? "Next Level" : "Completed!";
        DrawButtonStyled(btnTop, topText, 18,
            (Color){35, 70, 45, 255}, (Color){50, 100, 60, 255},
            (Color){60, 140, 80, 255}, (Color){100, 220, 120, 255},
            (Color){150, 220, 170, 255}, (Color){255, 255, 255, 255},
            topHover);
    }

    DrawButtonStyled(btnBot, "Main Menu", 18,
        (Color){40, 40, 60, 255}, (Color){60, 60, 80, 255},
        (Color){80, 80, 100, 255}, (Color){150, 150, 180, 255},
        (Color){180, 180, 210, 255}, (Color){255, 255, 255, 255},
        botHover);

    DrawTextCenteredX("ESC: Go Back", SCREEN_HEIGHT - 25, 14, (Color){100, 100, 140, 200});
}
