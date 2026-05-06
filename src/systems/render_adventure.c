#include "render.h"
#include "font.h"
#include "adventure.h"
#include "defs.h"
#include "textures.h"
#include <stdio.h>

// ============================================================================
//  Helper utilities (static — file-local only)
// ============================================================================

// ── Draw a texture using its full source rectangle ──────────────────────────
static void DrawTextureFull(Texture2D tex, int x, int y, int w, int h)
{
    DrawTexturePro(tex,
        (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
        (Rectangle){ (float)x, (float)y, (float)w, (float)h },
        (Vector2){ 0, 0 }, 0.0f, WHITE);
}

// ── Draw text horizontally centered on screen ──────────────────────────────
static void DrawTextCenteredX(const char *text, int y, int fontSize, Color color)
{
    int tw = GameMeasureText(text, fontSize);
    GameDrawText(text, (SCREEN_WIDTH - tw) / 2, y, fontSize, color);
}

// ── Draw a rounded rectangle button with centered text ─────────────────────
static void DrawButtonStyled(Rectangle btn, const char *text, int fontSize,
                              Color bg, Color bgHover,
                              Color border, Color borderHover,
                              Color textColor, Color textHover,
                              bool hover)
{
    Color cBg  = hover ? bgHover  : bg;
    Color cBrd = hover ? borderHover : border;
    Color cTxt = hover ? textHover : textColor;

    DrawRectangleRounded(btn, 0.2f, 6, cBg);
    DrawRectangleRoundedLines(btn, 0.2f, 6, cBrd);

    int tw = GameMeasureText(text, fontSize);
    GameDrawText(text,
                 (int)btn.x + (int)((btn.width  - tw)  / 2),
                 (int)btn.y + (int)((btn.height - fontSize) / 2),
                 fontSize, cTxt);
}

// ── Draw a gem icon with a count label centred below it ────────────────────
static void DrawGemCounter(Texture2D tex, int centerX, int topY, int gemSize,
                            const char *countText, int fontSize, int textYOffset,
                            Color textColor)
{
    DrawTextureFull(tex, centerX - gemSize / 2, topY, gemSize, gemSize);

    int tw = GameMeasureText(countText, fontSize);
    GameDrawText(countText, centerX - tw / 2, topY + textYOffset, fontSize, textColor);
}

// ── Draw a statistics line (label + value on the left, optional goal on right) ─
static void DrawStatLine(int leftX, int rightEdge, int y, int fontSize,
                          const char *label, int value, Color valueColor,
                          bool showGoal, int current, int goal)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%s: %d", label, value);
    GameDrawText(buf, leftX, y, fontSize, valueColor);

    if (showGoal) {
        snprintf(buf, sizeof(buf), "Hedef: %d", goal);
        int tw = GameMeasureText(buf, fontSize);
        Color goalColor = (current >= goal)
            ? (Color){50, 255, 100, 255}
            : (Color){255, 80, 80, 255};
        GameDrawText(buf, rightEdge - 30 - tw, y, fontSize, goalColor);
    }
}

// ============================================================================
//  Adventure Map
// ============================================================================
void RenderAdventureMap(GameState *state)
{
    // Top Left: Leave icon (logout)
    DrawTextureFull(gameTextures.logout, 15, 15, 32, 32);

    // Title
    DrawTextCenteredX("ADVENTURE", 30, 36, COLOR_TEXT_PRIMARY);

    // Level grid
    int totalRowWidth = LEVELS_PER_ROW * AMAP_BTN_SIZE + (LEVELS_PER_ROW - 1) * AMAP_BTN_GAP;
    int mapStartX = (SCREEN_WIDTH - totalRowWidth) / 2;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        int row = i / LEVELS_PER_ROW;
        int col = i % LEVELS_PER_ROW;
        int bx = mapStartX + col * (AMAP_BTN_SIZE + AMAP_BTN_GAP);
        int by = AMAP_START_Y + row * (AMAP_BTN_SIZE + AMAP_BTN_GAP + AMAP_BTN_LABEL_GAP);
        Rectangle btnRect = { bx, by, AMAP_BTN_SIZE, AMAP_BTN_SIZE };

        bool isUnlocked = state->adventureSave.levels[i].unlocked;
        bool isCompleted = state->adventureSave.levels[i].completed;
        bool hover = CheckCollisionPointRec(mouse, btnRect) && isUnlocked;

        // Button background
        Color btnColor;
        Color borderColor;
        if (!isUnlocked) {
            btnColor = COLOR_AMAP_LOCKED_BG;
            borderColor = COLOR_AMAP_LOCKED_BORDER;
        } else if (hover) {
            btnColor = COLOR_AMAP_UNLOCKED_HOVER;
            borderColor = COLOR_AMAP_UNLOCKED_BORDER;
        } else {
            btnColor = COLOR_AMAP_UNLOCKED_BG;
            borderColor = COLOR_AMAP_UNLOCKED_BORDER;
        }

        DrawRectangleRounded(btnRect, 0.2f, 6, btnColor);
        DrawRectangleRoundedLines(btnRect, 0.2f, 6, borderColor);

        // Level number
        char lvlBuf[8];
        sprintf(lvlBuf, "%d", i + 1);
        Color lvlColor = isUnlocked ? COLOR_TEXT_PRIMARY : COLOR_AMAP_LOCKED_NUMBER;
        int lnw = GameMeasureText(lvlBuf, 28);
        GameDrawText(lvlBuf, bx + (AMAP_BTN_SIZE - lnw) / 2, by + 20, 28, lvlColor);

        // Status indicator
        if (isCompleted) {
            // Green checkmark
            GameDrawText("✓", bx + AMAP_BTN_SIZE - 22, by + 5, 18, COLOR_AMAP_COMPLETED_TEXT);
        } else if (!isUnlocked) {
            // Lock icon below the level number
            int lockSize = 24;
            DrawTextureFull(gameTextures.lock,
                bx + (AMAP_BTN_SIZE - lockSize) / 2,
                by + AMAP_BTN_SIZE - lockSize - 8,
                lockSize, lockSize);
        }
    }

    // Hint at bottom
    DrawTextCenteredX("ESC: Ana Menu", SCREEN_HEIGHT - 30, 16, COLOR_TEXT_MUTED);
}

// ============================================================================
//  Adventure Play HUD
// ============================================================================
void RenderAdventurePlay(GameState *state)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[state->adventure.currentLevel];

    char buf[128];

    // Top Middle: Dynamic display based on level objectives
    int midY = 12;

    switch (level->goalType) {
        case GOAL_SCORE: {
            sprintf(buf, "%d", state->score);
            DrawTextCenteredX(buf, midY, 36, COLOR_TEXT_PRIMARY);
            break;
        }

        case GOAL_GEMS: {
            int gemSize = 32;
            sprintf(buf, "%d", state->adventure.goalDiamonds - state->adventure.collectedDiamonds);
            DrawGemCounter(gameTextures.diamond, SCREEN_WIDTH / 2, midY,
                           gemSize, buf, 22, gemSize + 4, COLOR_TEXT_PRIMARY);
            break;
        }

        case GOAL_MIXED_GEMS: {
            int gemSize = 28;
            int spacing = 60;
            int centerX = SCREEN_WIDTH / 2;

            sprintf(buf, "%d", state->adventure.goalDiamonds - state->adventure.collectedDiamonds);
            DrawGemCounter(gameTextures.diamond, centerX - spacing, midY,
                           gemSize, buf, 18, gemSize + 4, COLOR_TEXT_PRIMARY);

            sprintf(buf, "%d", state->adventure.goalEmeralds - state->adventure.collectedEmeralds);
            DrawGemCounter(gameTextures.emerald, centerX + spacing, midY,
                           gemSize, buf, 18, gemSize + 4, COLOR_TEXT_PRIMARY);
            break;
        }

        case GOAL_MIXED_ALL: {
            int gemSize = 24;
            int spacing = 80;
            int centerX = SCREEN_WIDTH / 2;

            // Score (middle)
            sprintf(buf, "%d", state->score);
            DrawTextCenteredX(buf, midY + 4, 28, COLOR_TEXT_PRIMARY);

            // Diamond (left)
            sprintf(buf, "%d", state->adventure.goalDiamonds - state->adventure.collectedDiamonds);
            DrawGemCounter(gameTextures.diamond, centerX - spacing, midY + 2,
                           gemSize, buf, 16, gemSize + 6, COLOR_TEXT_PRIMARY);

            // Emerald (right)
            sprintf(buf, "%d", state->adventure.goalEmeralds - state->adventure.collectedEmeralds);
            DrawGemCounter(gameTextures.emerald, centerX + spacing, midY + 2,
                           gemSize, buf, 16, gemSize + 6, COLOR_TEXT_PRIMARY);
            break;
        }
    }

    // Render board
    RenderBoard(state);

    // Render piece slots
    RenderPieceSlots(state);
}

// ============================================================================
//  Adventure Result Screen
// ============================================================================
void RenderAdventureResult(GameState *state)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[state->adventure.currentLevel];

    // Draw the game board underneath (semi-transparent overlay on top)
    RenderAdventurePlay(state);

    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    // ─── Card background ──────────────────────────────────────────────────────
    int cardW = 380, cardH = 400;
    int cardX = (SCREEN_WIDTH - cardW) / 2;
    int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
    DrawRectangleRounded((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){20, 25, 50, 235});
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){60, 70, 110, 180});

    char buf[128];
    Vector2 mouse = GetMousePosition();

    // ─── Title ────────────────────────────────────────────────────────────────
    if (state->adventure.levelFailed) {
        DrawTextCenteredX("SEVIYE BASARISIZ!", cardY + 30, 32, (Color){255, 80, 80, 255});
    } else {
        DrawTextCenteredX("SEVIYE TAMAMLANDI!", cardY + 30, 32, (Color){50, 255, 100, 255});
    }

    // ─── Level info ───────────────────────────────────────────────────────────
    sprintf(buf, "Level %d", state->adventure.currentLevel + 1);
    DrawTextCenteredX(buf, cardY + 75, 20, (Color){180, 150, 255, 255});

    // ─── Stats ────────────────────────────────────────────────────────────────
    int statY = cardY + 120;
    int statX = cardX + 30;
    int cardRight = cardX + cardW;
    int statSpacing = 35;
    int statFontSize = 18;

    // Score
    bool scoreGoalShown = (level->goalType == GOAL_SCORE || level->goalType == GOAL_MIXED_ALL);
    Color scoreColor = (!scoreGoalShown && state->adventure.levelFailed)
        ? (Color){150, 150, 170, 255} : (Color){255, 255, 255, 255};
    DrawStatLine(statX, cardRight, statY, statFontSize, "Puan", state->score, scoreColor,
                 scoreGoalShown, state->score, state->adventure.goalScore);

    // Diamonds
    if (level->goalType == GOAL_GEMS || level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
        statY += statSpacing;
        DrawStatLine(statX, cardRight, statY, statFontSize, "Elmas", state->adventure.collectedDiamonds,
                     (Color){100, 200, 255, 255}, true,
                     state->adventure.collectedDiamonds, state->adventure.goalDiamonds);
    }

    // Emeralds
    if (level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
        statY += statSpacing;
        DrawStatLine(statX, cardRight, statY, statFontSize, "Zumrut", state->adventure.collectedEmeralds,
                     (Color){50, 220, 100, 255}, true,
                     state->adventure.collectedEmeralds, state->adventure.goalEmeralds);
    }

    // ─── Buttons (vertical layout) ────────────────────────────────────────────
    const int btnW = 220, btnH = 50;
    const int btnGap = 15;
    const int btnX = (SCREEN_WIDTH - btnW) / 2;
    const int topBtnY = cardY + cardH - 140;
    const int botBtnY = topBtnY + btnH + btnGap;

    Rectangle btnTop = { btnX, topBtnY, btnW, btnH };
    Rectangle btnBot = { btnX, botBtnY, btnW, btnH };

    bool topHover = CheckCollisionPointRec(mouse, btnTop);
    bool botHover = CheckCollisionPointRec(mouse, btnBot);

    if (state->adventure.levelFailed) {
        DrawButtonStyled(btnTop, "Try Again", 18,
            (Color){55, 40, 85, 255}, (Color){80, 50, 120, 255},
            (Color){100, 70, 150, 255}, (Color){180, 120, 255, 255},
            (Color){170, 150, 200, 255}, (Color){255, 255, 255, 255},
            topHover);
        DrawButtonStyled(btnBot, "Main Menu", 18,
            (Color){40, 40, 60, 255}, (Color){60, 60, 80, 255},
            (Color){80, 80, 100, 255}, (Color){150, 150, 180, 255},
            (Color){180, 180, 210, 255}, (Color){255, 255, 255, 255},
            botHover);
    } else {
        const char *topText = (state->adventure.currentLevel + 1 < TOTAL_LEVELS)
                              ? "Next Level" : "Completed!";
        DrawButtonStyled(btnTop, topText, 18,
            (Color){35, 70, 45, 255}, (Color){50, 100, 60, 255},
            (Color){60, 140, 80, 255}, (Color){100, 220, 120, 255},
            (Color){150, 220, 170, 255}, (Color){255, 255, 255, 255},
            topHover);
        DrawButtonStyled(btnBot, "Main Menu", 18,
            (Color){40, 40, 60, 255}, (Color){60, 60, 80, 255},
            (Color){80, 80, 100, 255}, (Color){150, 150, 180, 255},
            (Color){180, 180, 210, 255}, (Color){255, 255, 255, 255},
            botHover);
    }

    // Hint at bottom
    DrawTextCenteredX("ESC: Go Back", SCREEN_HEIGHT - 25, 14, (Color){100, 100, 140, 200});
}