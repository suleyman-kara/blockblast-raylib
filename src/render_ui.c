#include "render.h"
#include "level.h"
#include "defs.h"
#include "textures.h"
#include "ui_layout.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


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
            (Rectangle){ 15, 12, (float)crownSize, (float)crownSize },
            (Vector2){ 0, 0 }, 0.0f, WHITE);

        sprintf(buf, "%d", state->highScore);
        DrawTextEx(gameFont, buf, (Vector2){15 + crownSize + 8, 16}, 20.0f, 1.0f, (Color){255, 220, 50, 255});

        sprintf(buf, "%d", state->score);
        int scoreW = (int)MeasureTextEx(gameFont, buf, 40.0f, 1.0f).x;
        DrawTextEx(gameFont, buf, (Vector2){(SCREEN_WIDTH - scoreW) / 2.0f, 40}, 40.0f, 1.0f, COLOR_TEXT_PRIMARY);

        if (state->combo > 1) {
            sprintf(buf, "COMBO x%d", state->combo);
            int cw = (int)MeasureTextEx(gameFont, buf, 18.0f, 1.0f).x;
            DrawTextEx(gameFont, buf, (Vector2){(SCREEN_WIDTH - cw) / 2.0f, 80}, 18.0f, 1.0f, (Color){255, 220, 50, 255});
        }
    } else {
        // Adventure mode: show only non-zero targets
        int midY = 45;
        bool hasScore = (def->targetScore > 0);
        bool hasDiamond = (def->targetDiamonds > 0);
        bool hasEmerald = (def->targetEmeralds > 0);

        if (hasScore && !hasDiamond && !hasEmerald) {
            // Score only
            sprintf(buf, "%d / %d", state->score, def->targetScore);
            DrawTextCenteredX(buf, midY, 36, COLOR_TEXT_PRIMARY);
        } else if (!hasScore && hasDiamond && !hasEmerald) {
            // Diamond only
            int gemSize = 36;
            int remaining = def->targetDiamonds - state->level.collectedDiamonds;
            if (remaining < 0) remaining = 0;
            sprintf(buf, "%d", remaining);
            DrawGemCounter(gameTextures.diamond, SCREEN_WIDTH / 2, midY - 6,
                           gemSize, buf, 26, gemSize + 4, COLOR_TEXT_PRIMARY);
        } else {
            // Mixed: show all non-zero targets
            int gemSize = 32;
            int spacing = 125; // increased spacing
            int centerX = SCREEN_WIDTH / 2;

            if (hasScore) {
                sprintf(buf, "%d / %d", state->score, def->targetScore);
                DrawTextCenteredX(buf, midY + 4, 28, COLOR_TEXT_PRIMARY);
            }

            if (hasDiamond) {
                int remaining = def->targetDiamonds - state->level.collectedDiamonds;
                if (remaining < 0) remaining = 0;
                sprintf(buf, "%d", remaining);
                DrawGemCounter(gameTextures.diamond, centerX - spacing, midY - 4,
                               gemSize, buf, 22, gemSize + 6, COLOR_TEXT_PRIMARY);
            }

            if (hasEmerald) {
                int remaining = def->targetEmeralds - state->level.collectedEmeralds;
                if (remaining < 0) remaining = 0;
                sprintf(buf, "%d", remaining);
                DrawGemCounter(gameTextures.emerald, centerX + spacing, midY - 4,
                               gemSize, buf, 22, gemSize + 6, COLOR_TEXT_PRIMARY);
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

        bool isUnlocked = LevelIsUnlocked(state->unlockedLevel, i + 1);
        bool isCompleted = LevelIsCompleted(state->unlockedLevel, i + 1);
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

    if (state->unlockedLevel > TOTAL_LEVELS) {
        int lastRow = (TOTAL_LEVELS - 1) / LEVELS_PER_ROW;
        int botY = AMAP_START_Y + lastRow * (AMAP_BTN_SIZE + AMAP_BTN_GAP + AMAP_BTN_LABEL_GAP) + AMAP_BTN_SIZE + 40;
        DrawTextCenteredX("Completed!", botY, 28, COLOR_AMAP_COMPLETED_TEXT);
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
#include <string.h>

// ─── Settings overlay colors
// ──────────────────────────────────────────────────
#define COLOR_SETTINGS_OVERLAY_BG ((Color){0, 0, 0, 160})
#define COLOR_SETTINGS_CARD_BG ((Color){20, 25, 50, 235})
#define COLOR_SETTINGS_CARD_BORDER ((Color){60, 70, 110, 180})
#define COLOR_SETTINGS_SEPARATOR ((Color){80, 80, 120, 200})
#define COLOR_SETTINGS_SELECTED_TEXT ((Color){255, 220, 50, 255})
#define COLOR_SETTINGS_UNSELECTED_TEXT ((Color){180, 180, 210, 255})
#define COLOR_SETTINGS_FOOTER_HINT ((Color){100, 100, 140, 200})

// ─── Menu Buttons — Classic Mode colors
// ───────────────────────────────────────
#define COLOR_BTN_STD_BG ((Color){60, 100, 180, 255})
#define COLOR_BTN_STD_BG_HOVER ((Color){80, 130, 210, 255})
#define COLOR_BTN_STD_BORDER ((Color){100, 150, 220, 255})
#define COLOR_BTN_STD_BORDER_HOVER ((Color){140, 200, 255, 255})
#define COLOR_BTN_STD_TEXT ((Color){220, 230, 250, 255})
#define COLOR_BTN_STD_TEXT_HOVER ((Color){255, 255, 255, 255})

// ─── Menu Buttons — Adventure Mode colors
// ─────────────────────────────────────
#define COLOR_BTN_ADV_BG ((Color){55, 40, 85, 255})
#define COLOR_BTN_ADV_BG_HOVER ((Color){80, 60, 120, 255})
#define COLOR_BTN_ADV_BORDER ((Color){100, 70, 150, 255})
#define COLOR_BTN_ADV_BORDER_HOVER ((Color){180, 120, 255, 255})
#define COLOR_BTN_ADV_TEXT ((Color){170, 150, 200, 255})
#define COLOR_BTN_ADV_TEXT_HOVER ((Color){230, 210, 255, 255})

// ─── Menu Buttons — Quit colors
// ──────────────────────────────
#define COLOR_BTN_QUIT_BG ((Color){140, 40, 40, 255})
#define COLOR_BTN_QUIT_BG_HOVER ((Color){180, 60, 60, 255})
#define COLOR_BTN_QUIT_BORDER ((Color){160, 60, 60, 255})
#define COLOR_BTN_QUIT_BORDER_HOVER ((Color){220, 80, 80, 255})
#define COLOR_BTN_QUIT_TEXT ((Color){220, 180, 180, 255})
#define COLOR_BTN_QUIT_TEXT_HOVER ((Color){255, 220, 220, 255})

// ─── Helper: draw text centered X ────────────────────────────────────────────
static void DrawMenuTextCenteredX(const char *text, int y, int fontSize, Color color)
{
    int tw = (int)MeasureTextEx(gameFont, text, (float)fontSize, 1.0f).x;
    DrawTextEx(gameFont, text, (Vector2){(SCREEN_WIDTH - tw) / 2.0f, (float)y},
               (float)fontSize, 1.0f, color);
}

// ─── Helper: draw styled button ──────────────────────────────────────────────
static void DrawMenuButtonStyled(Rectangle btn, const char *text, int fontSize,
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

// ----- Menu screen -----
void RenderMenu(GameState *state) {
  // Draw Logo (Scaled to fit the screen)
  float targetW = SCREEN_WIDTH - 80.0f; // 40px margin on each side
  float scale = targetW / (float)gameTextures.logo.width;
  float targetH = (float)gameTextures.logo.height * scale;
  float logoX = (SCREEN_WIDTH - targetW) / 2.0f;
  float logoY = 50.0f; // Slightly higher to account for scaling
  DrawTexturePro(gameTextures.logo,
                 (Rectangle){0, 0, (float)gameTextures.logo.width, (float)gameTextures.logo.height},
                 (Rectangle){logoX, logoY, targetW, targetH},
                 (Vector2){0, 0}, 0.0f, WHITE);

  Vector2 mouse = GetMousePosition();

  // --- Nickname (top-left) ---
  if (strlen(state->nickname) > 0) {
      DrawTextEx(gameFont, state->nickname, (Vector2){10, 10}, 14.0f, 1.0f, COLOR_TEXT_MUTED);
  }

  // --- Gear Icon (top-right) for menu settings ---
  RenderGearIcon();

  // --- Standard Mode Button ---
  Rectangle stdBtn = {BTN_X, MENU_STD_Y, BTN_W, BTN_H};
  bool stdHover = CheckCollisionPointRec(mouse, stdBtn);

  Color stdBg = stdHover ? COLOR_BTN_STD_BG_HOVER : COLOR_BTN_STD_BG;
  Color stdBorder =
      stdHover ? COLOR_BTN_STD_BORDER_HOVER : COLOR_BTN_STD_BORDER;

  DrawRectangleRounded(stdBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, stdBg);
  DrawRectangleRoundedLines(stdBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                            stdBorder);

  const char *stdText = "Classic Mode";
  int stw = (int)MeasureTextEx(gameFont, stdText, 22.0f, 1.0f).x;
  Color stdTextColor = stdHover ? COLOR_BTN_STD_TEXT_HOVER : COLOR_BTN_STD_TEXT;
  DrawTextEx(gameFont, stdText,
             (Vector2){(float)(BTN_X + (BTN_W - stw) / 2),
                       (float)(MENU_STD_Y + (BTN_H - 22) / 2)},
             22.0f, 1.0f, stdTextColor);

  // --- Adventure Mode Button ---
  Rectangle advBtn = {BTN_X, MENU_ADV_Y, BTN_W, BTN_H};
  bool advHover = CheckCollisionPointRec(mouse, advBtn);

  Color advBg = advHover ? COLOR_BTN_ADV_BG_HOVER : COLOR_BTN_ADV_BG;
  Color advBorder =
      advHover ? COLOR_BTN_ADV_BORDER_HOVER : COLOR_BTN_ADV_BORDER;

  DrawRectangleRounded(advBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, advBg);
  DrawRectangleRoundedLines(advBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                            advBorder);

  const char *advText = "Adventure Mode";
  int atw = (int)MeasureTextEx(gameFont, advText, 22.0f, 1.0f).x;
  Color advTextColor = advHover ? COLOR_BTN_ADV_TEXT_HOVER : COLOR_BTN_ADV_TEXT;
  DrawTextEx(gameFont, advText,
             (Vector2){(float)(BTN_X + (BTN_W - atw) / 2),
                       (float)(MENU_ADV_Y + (BTN_H - 22) / 2)},
             22.0f, 1.0f, advTextColor);

  if (state->unlockedLevel > TOTAL_LEVELS) {
      const char *compText = "Completed";
      int ctw = (int)MeasureTextEx(gameFont, compText, 14.0f, 1.0f).x;
      DrawTextEx(gameFont, compText,
                 (Vector2){(float)(BTN_X + (BTN_W - ctw) / 2),
                           (float)(MENU_ADV_Y + BTN_H + 5)},
                 14.0f, 1.0f, (Color){ 50,  255, 100, 255 });
  }

  // --- Quit Button ---
  Rectangle quitBtn = {BTN_X, MENU_QUIT_Y, BTN_W, BTN_H};
  bool quitHover = CheckCollisionPointRec(mouse, quitBtn);
  DrawMenuButtonStyled(quitBtn, "Quit", 22,
                   COLOR_BTN_QUIT_BG, COLOR_BTN_QUIT_BG_HOVER,
                   COLOR_BTN_QUIT_BORDER, COLOR_BTN_QUIT_BORDER_HOVER,
                   COLOR_BTN_QUIT_TEXT, COLOR_BTN_QUIT_TEXT_HOVER,
                   quitHover);

  // --- Scoreboard Button (bottom-left) ---
  Rectangle sbBtn = {10, SCREEN_HEIGHT - 40, 100, 30};
  bool sbHover = CheckCollisionPointRec(mouse, sbBtn);
  Color sbBg = sbHover ? (Color){60, 90, 140, 255} : (Color){40, 60, 100, 255};
  DrawRectangleRec(sbBtn, sbBg);
  DrawRectangleLinesEx(sbBtn, 1, (Color){60, 80, 130, 255});
  int sbW = MeasureText("Scoreboard", 12);
  DrawText("Scoreboard", sbBtn.x + (sbBtn.width - sbW) / 2, sbBtn.y + (sbBtn.height - 12) / 2, 12, WHITE);

  // --- Game Simulation Decoration ---
  int s = 35; // Block size for mini grid
  int startX = (SCREEN_WIDTH - (4 * s)) / 2;
  int startY = 560;

  // 1. Draw Grid Background
  DrawRectangle(startX - 4, startY - 4, 4*s + 8, 4*s + 8, (Color){ 50,  50,  70, 255 });

  // 2. Draw empty cells
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      DrawRectangle(startX + c*s + 1, startY + r*s + 1, s - 2, s - 2, (Color){ 40,  40,  60, 255 });
    }
  }

  // 3. Placed Piece: Blue L-shape (left side)
  Color colBlue = PIECE_COLORS[4];
  DrawBlockBeveled(startX + 0*s, startY + 1*s, s, colBlue); // Row 1, Col 0
  DrawBlockBeveled(startX + 0*s, startY + 2*s, s, colBlue); // Row 2, Col 0
  DrawBlockBeveled(startX + 0*s, startY + 3*s, s, colBlue); // Row 3, Col 0
  DrawBlockBeveled(startX + 1*s, startY + 3*s, s, colBlue); // Row 3, Col 1

  // 4. Placed Piece: Yellow 2x2 Square (bottom right)
  Color colYellow = PIECE_COLORS[2];
  DrawBlockBeveled(startX + 2*s, startY + 2*s, s, colYellow); // Row 2, Col 2
  DrawBlockBeveled(startX + 3*s, startY + 2*s, s, colYellow); // Row 2, Col 3
  DrawBlockBeveled(startX + 2*s, startY + 3*s, s, colYellow); // Row 3, Col 2
  DrawBlockBeveled(startX + 3*s, startY + 3*s, s, colYellow); // Row 3, Col 3

  // 5. Hovering Piece: Red 3x1 horizontal
  Color colRed = PIECE_COLORS[1];
  Color ghostRed = colRed;
  ghostRed.a = 80;

  // Ghost indicator on the grid
  DrawBlockBeveled(startX + 1*s, startY + 0*s, s, ghostRed); // Row 0, Col 1
  DrawBlockBeveled(startX + 2*s, startY + 0*s, s, ghostRed); // Row 0, Col 2
  DrawBlockBeveled(startX + 3*s, startY + 0*s, s, ghostRed); // Row 0, Col 3

  // Actual hovering piece (offset upwards to simulate dragging)
  int hoverOffsetY = -18;
  int hoverOffsetX = 6;
  DrawBlockBeveled(startX + 1*s + hoverOffsetX, startY + 0*s + hoverOffsetY, s, colRed);
  DrawBlockBeveled(startX + 2*s + hoverOffsetX, startY + 0*s + hoverOffsetY, s, colRed);
  DrawBlockBeveled(startX + 3*s + hoverOffsetX, startY + 0*s + hoverOffsetY, s, colRed);
}

// ----- Settings screen (overlay with framed card) - for in-game settings -----
void RenderSettings(GameState *state) {
  SettingsLayout layout = GetSettingsLayout(false);

  // Semi-transparent dark overlay
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SETTINGS_OVERLAY_BG);

  // Settings card frame
  int cardX = layout.cardX;
  int cardY = layout.cardY;

  // Card background
  DrawRectangleRounded(
      (Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT},
      0.15f, 8, COLOR_SETTINGS_CARD_BG);
  DrawRectangleRoundedLines(
      (Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT},
      0.15f, 8, COLOR_SETTINGS_CARD_BORDER);

  // Title
  const char *title = "SETTINGS";
  int tw =
      (int)MeasureTextEx(gameFont, title, (float)SETTINGS_TITLE_FONT_SIZE, 1.0f)
          .x;
  DrawTextEx(gameFont, title,
             (Vector2){(SCREEN_WIDTH - tw) / 2.0f, (float)(cardY + 25)},
             (float)SETTINGS_TITLE_FONT_SIZE, 1.0f, WHITE);

  // Separator line
  DrawLineEx((Vector2){cardX + 40, cardY + 65},
             (Vector2){cardX + SETTINGS_CARD_WIDTH - 40, cardY + 65}, 1.5f,
             COLOR_SETTINGS_SEPARATOR);

  Vector2 mouse = GetMousePosition();

  // ─── SFX & Music Icons (horizontal) ──────────────────────────────────────
  // SFX icon
  int sfxIconX = (int)layout.sfxIcon.x;
  int iconAreaY = (int)layout.sfxIcon.y;
  DrawTexturePro(gameTextures.waveSound,
                 (Rectangle){0, 0, (float)gameTextures.waveSound.width,
                             (float)gameTextures.waveSound.height},
                 (Rectangle){(float)sfxIconX, (float)iconAreaY,
                             (float)SETTINGS_ICON_SIZE,
                             (float)SETTINGS_ICON_SIZE},
                 (Vector2){0, 0}, 0.0f, WHITE);

  // Red cross overlay if SFX is off (single diagonal: bottom-left to top-right)
  if (!state->sound.sfxEnabled) {
    int cx = sfxIconX;
    int cy = iconAreaY;
    DrawLineEx((Vector2){cx + 2, cy + SETTINGS_ICON_SIZE - 2},
               (Vector2){cx + SETTINGS_ICON_SIZE - 2, cy + 2}, 5.0f, RED);
  }

  // SFX label below icon
  const char *sfxLabel = "SFX";
  int sfxLabelW = (int)MeasureTextEx(gameFont, sfxLabel, 14.0f, 1.0f).x;
  DrawTextEx(gameFont, sfxLabel,
             (Vector2){(float)(sfxIconX + (SETTINGS_ICON_SIZE - sfxLabelW) / 2),
                       (float)(iconAreaY + SETTINGS_ICON_SIZE + 4)},
             14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

  // Music icon
  int musicIconX = (int)layout.musicIcon.x;
  DrawTexturePro(gameTextures.musicalNote,
                 (Rectangle){0, 0, (float)gameTextures.musicalNote.width,
                             (float)gameTextures.musicalNote.height},
                 (Rectangle){(float)musicIconX, (float)iconAreaY,
                             (float)SETTINGS_ICON_SIZE,
                             (float)SETTINGS_ICON_SIZE},
                 (Vector2){0, 0}, 0.0f, WHITE);

  // Red cross overlay if Music is off (single diagonal: bottom-left to
  // top-right)
  if (!state->sound.musicEnabled) {
    int cx = musicIconX;
    int cy = iconAreaY;
    DrawLineEx((Vector2){cx + 2, cy + SETTINGS_ICON_SIZE - 2},
               (Vector2){cx + SETTINGS_ICON_SIZE - 2, cy + 2}, 5.0f, RED);
  }

  // Music label below icon
  const char *musicLabel = "Music";
  int musicLabelW = (int)MeasureTextEx(gameFont, musicLabel, 14.0f, 1.0f).x;
  DrawTextEx(
      gameFont, musicLabel,
      (Vector2){(float)(musicIconX + (SETTINGS_ICON_SIZE - musicLabelW) / 2),
                (float)(iconAreaY + SETTINGS_ICON_SIZE + 4)},
      14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

  // ─── Replay Button ────────────────────────────────────────────────────────
  int replayBtnY = (int)layout.firstButton.y;
  Rectangle replayBtn = layout.firstButton;
  bool replayHover = CheckCollisionPointRec(mouse, replayBtn);

  Color replayBg =
      replayHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255};
  Color replayBorder =
      replayHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255};
  DrawRectangleRounded(replayBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                       replayBg);
  DrawRectangleRoundedLines(replayBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                            replayBorder);

  // Replay icon at left of text
  int replayIconSize = 20;
  DrawTexturePro(gameTextures.replay,
                 (Rectangle){0, 0, (float)gameTextures.replay.width,
                             (float)gameTextures.replay.height},
                 (Rectangle){(float)(BTN_X + 15),
                             (float)(replayBtnY + (BTN_H - replayIconSize) / 2),
                             (float)replayIconSize, (float)replayIconSize},
                 (Vector2){0, 0}, 0.0f, WHITE);

  const char *replayText = "Replay";
  int replayTextW = (int)MeasureTextEx(gameFont, replayText, 18.0f, 1.0f).x;
  DrawTextEx(
      gameFont, replayText,
      (Vector2){(float)(BTN_X + 15 + replayIconSize + 8 +
                        (BTN_W - 15 - replayIconSize - 8 - replayTextW) / 2),
                (float)(replayBtnY + (BTN_H - 18) / 2)},
      18.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

  // ─── Home Button ──────────────────────────────────────────────────────────
  int homeBtnY = (int)layout.secondButton.y;
  Rectangle homeBtn = layout.secondButton;
  bool homeHover = CheckCollisionPointRec(mouse, homeBtn);

  Color homeBg =
      homeHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255};
  Color homeBorder =
      homeHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255};
  DrawRectangleRounded(homeBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, homeBg);
  DrawRectangleRoundedLines(homeBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                            homeBorder);

  // Home icon at left of text
  int homeIconSize = 20;
  DrawTexturePro(gameTextures.home,
                 (Rectangle){0, 0, (float)gameTextures.home.width,
                             (float)gameTextures.home.height},
                 (Rectangle){(float)(BTN_X + 15),
                             (float)(homeBtnY + (BTN_H - homeIconSize) / 2),
                             (float)homeIconSize, (float)homeIconSize},
                 (Vector2){0, 0}, 0.0f, WHITE);

  const char *homeText = "Home";
  int homeTextW = (int)MeasureTextEx(gameFont, homeText, 18.0f, 1.0f).x;
  DrawTextEx(gameFont, homeText,
             (Vector2){(float)(BTN_X + 15 + homeIconSize + 8 +
                               (BTN_W - 15 - homeIconSize - 8 - homeTextW) / 2),
                       (float)(homeBtnY + (BTN_H - 18) / 2)},
             18.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

  // Footer hint
  const char *hint = "ESC to go back";
  int hw = (int)MeasureTextEx(gameFont, hint, 14.0f, 1.0f).x;
  DrawTextEx(gameFont, hint,
             (Vector2){(SCREEN_WIDTH - hw) / 2.0f,
                       (float)(cardY + SETTINGS_CARD_HEIGHT - 30)},
             14.0f, 1.0f, COLOR_SETTINGS_FOOTER_HINT);
}

// ----- Menu Settings screen (from main menu gear icon) -----
void RenderMenuSettings(GameState *state)
{
    SettingsLayout layout = GetSettingsLayout(true);

    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SETTINGS_OVERLAY_BG);

    // Settings card frame
    int cardX = layout.cardX;
    int cardY = layout.cardY;

    // Card background
    DrawRectangleRounded(
        (Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT},
        0.15f, 8, COLOR_SETTINGS_CARD_BG);
    DrawRectangleRoundedLines(
        (Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT},
        0.15f, 8, COLOR_SETTINGS_CARD_BORDER);

    // Title
    const char *title = "SETTINGS";
    int tw = (int)MeasureTextEx(gameFont, title, (float)SETTINGS_TITLE_FONT_SIZE, 1.0f).x;
    DrawTextEx(gameFont, title,
               (Vector2){(SCREEN_WIDTH - tw) / 2.0f, (float)(cardY + 25)},
               (float)SETTINGS_TITLE_FONT_SIZE, 1.0f, WHITE);

    // Separator line
    DrawLineEx((Vector2){cardX + 40, cardY + 65},
               (Vector2){cardX + SETTINGS_CARD_WIDTH - 40, cardY + 65}, 1.5f,
               COLOR_SETTINGS_SEPARATOR);

    Vector2 mouse = GetMousePosition();

    // ─── SFX & Music Icons (horizontal) ──────────────────────────────────────
    // SFX icon
    int sfxIconX = (int)layout.sfxIcon.x;
    int iconAreaY = (int)layout.sfxIcon.y;
    DrawTexturePro(gameTextures.waveSound,
                   (Rectangle){0, 0, (float)gameTextures.waveSound.width,
                               (float)gameTextures.waveSound.height},
                   (Rectangle){(float)sfxIconX, (float)iconAreaY,
                               (float)SETTINGS_ICON_SIZE,
                               (float)SETTINGS_ICON_SIZE},
                   (Vector2){0, 0}, 0.0f, WHITE);

    if (!state->sound.sfxEnabled) {
        int cx = sfxIconX;
        int cy = iconAreaY;
        DrawLineEx((Vector2){cx + 2, cy + SETTINGS_ICON_SIZE - 2},
                   (Vector2){cx + SETTINGS_ICON_SIZE - 2, cy + 2}, 5.0f, RED);
    }

    const char *sfxLabel = "SFX";
    int sfxLabelW = (int)MeasureTextEx(gameFont, sfxLabel, 14.0f, 1.0f).x;
    DrawTextEx(gameFont, sfxLabel,
               (Vector2){(float)(sfxIconX + (SETTINGS_ICON_SIZE - sfxLabelW) / 2),
                         (float)(iconAreaY + SETTINGS_ICON_SIZE + 4)},
               14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    // Music icon
    int musicIconX = (int)layout.musicIcon.x;
    DrawTexturePro(gameTextures.musicalNote,
                   (Rectangle){0, 0, (float)gameTextures.musicalNote.width,
                               (float)gameTextures.musicalNote.height},
                   (Rectangle){(float)musicIconX, (float)iconAreaY,
                               (float)SETTINGS_ICON_SIZE,
                               (float)SETTINGS_ICON_SIZE},
                   (Vector2){0, 0}, 0.0f, WHITE);

    if (!state->sound.musicEnabled) {
        int cx = musicIconX;
        int cy = iconAreaY;
        DrawLineEx((Vector2){cx + 2, cy + SETTINGS_ICON_SIZE - 2},
                   (Vector2){cx + SETTINGS_ICON_SIZE - 2, cy + 2}, 5.0f, RED);
    }

    const char *musicLabel = "Music";
    int musicLabelW = (int)MeasureTextEx(gameFont, musicLabel, 14.0f, 1.0f).x;
    DrawTextEx(gameFont, musicLabel,
               (Vector2){(float)(musicIconX + (SETTINGS_ICON_SIZE - musicLabelW) / 2),
                         (float)(iconAreaY + SETTINGS_ICON_SIZE + 4)},
               14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    // ─── Change Nickname Button ──────────────────────────────────────────────
    Rectangle nickBtn = layout.firstButton;
    bool nickHover = CheckCollisionPointRec(mouse, nickBtn);
    DrawMenuButtonStyled(nickBtn, "Change Nickname", 18,
                     (Color){55, 40, 85, 255}, (Color){80, 50, 120, 255},
                     (Color){100, 70, 150, 255}, (Color){180, 120, 255, 255},
                     (Color){170, 150, 200, 255}, (Color){255, 255, 255, 255},
                     nickHover);

    // ─── Reset Button (progress + high score, no scoreboard) ─────────────────
    Rectangle resetBtn = layout.secondButton;
    bool resetHover = CheckCollisionPointRec(mouse, resetBtn);
    DrawMenuButtonStyled(resetBtn, "Reset Progress", 18,
                     (Color){140, 40, 40, 255}, (Color){180, 60, 60, 255},
                     (Color){160, 60, 60, 255}, (Color){220, 80, 80, 255},
                     (Color){220, 180, 180, 255}, (Color){255, 220, 220, 255},
                     resetHover);

    // ─── Home Button ─────────────────────────────────────────────────────────
    Rectangle homeBtn = layout.thirdButton;
    bool homeHover = CheckCollisionPointRec(mouse, homeBtn);
    DrawMenuButtonStyled(homeBtn, "Home", 18,
                     (Color){40, 40, 60, 255}, (Color){60, 60, 80, 255},
                     (Color){80, 80, 100, 255}, (Color){150, 150, 180, 255},
                     (Color){180, 180, 210, 255}, (Color){255, 255, 255, 255},
                     homeHover);

    // Footer hint
    const char *hint = "ESC to go back";
    int hw = (int)MeasureTextEx(gameFont, hint, 14.0f, 1.0f).x;
    DrawTextEx(gameFont, hint,
               (Vector2){(SCREEN_WIDTH - hw) / 2.0f,
                         (float)(cardY + SETTINGS_CARD_HEIGHT - 30)},
               14.0f, 1.0f, COLOR_SETTINGS_FOOTER_HINT);
}

// ----- Nickname Input Screen -----
void RenderNickname(GameState *state)
{
    // Dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    // Card
    int cardW = 380, cardH = 300;
    int cardX = (SCREEN_WIDTH - cardW) / 2;
    int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
    DrawRectangleRounded((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){20, 25, 50, 235});
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){60, 70, 110, 180});

    DrawMenuTextCenteredX("Enter Your Nickname", cardY + 30, 28, WHITE);

    // Input box
    int inputW = 280, inputH = 50;
    int inputX = (SCREEN_WIDTH - inputW) / 2;
    int inputY = cardY + 100;
    Rectangle inputRect = {inputX, inputY, inputW, inputH};
    DrawRectangleRounded(inputRect, 0.1f, 6, (Color){30, 35, 60, 255});
    DrawRectangleRoundedLines(inputRect, 0.1f, 6, (Color){80, 90, 140, 255});

    // Draw nickname text with blinking cursor
    char displayBuf[64];
    sprintf(displayBuf, "%s%c", state->nicknameInput, (GetTime() * 2.0f < 1.0f) ? '_' : ' ');
    int textW = (int)MeasureTextEx(gameFont, displayBuf, 24.0f, 1.0f).x;
    DrawTextEx(gameFont, displayBuf,
               (Vector2){(float)(inputX + (inputW - textW) / 2), (float)(inputY + (inputH - 24) / 2)},
               24.0f, 1.0f, COLOR_TEXT_PRIMARY);

    // Confirm button
    Rectangle confirmBtn = {BTN_X, inputY + inputH + 30, BTN_W, BTN_H};
    bool confirmHover = CheckCollisionPointRec(GetMousePosition(), confirmBtn);
    bool hasText = (strlen(state->nicknameInput) > 0);
    DrawMenuButtonStyled(confirmBtn, "Confirm", 20,
                     hasText ? (Color){35, 70, 45, 255} : (Color){40, 40, 60, 255},
                     hasText ? (Color){50, 100, 60, 255} : (Color){60, 60, 80, 255},
                     hasText ? (Color){60, 140, 80, 255} : (Color){80, 80, 100, 255},
                     hasText ? (Color){100, 220, 120, 255} : (Color){150, 150, 180, 255},
                     hasText ? (Color){150, 220, 170, 255} : (Color){180, 180, 210, 255},
                     hasText ? (Color){255, 255, 255, 255} : (Color){255, 255, 255, 255},
                     confirmHover && hasText);

    DrawMenuTextCenteredX("ESC: Go Back", SCREEN_HEIGHT - 30, 14, (Color){100, 100, 140, 200});
}

// ----- Scoreboard Screen -----
void RenderScoreboard(GameState *state)
{
    // Dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    // Card
    int cardW = 380, cardH = 480;
    int cardX = (SCREEN_WIDTH - cardW) / 2;
    int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
    DrawRectangleRounded((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){20, 25, 50, 235});
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){60, 70, 110, 180});

    DrawMenuTextCenteredX("SCOREBOARD", cardY + 25, 30, WHITE);

    // Separator
    DrawLineEx((Vector2){cardX + 40, cardY + 60},
               (Vector2){cardX + cardW - 40, cardY + 60}, 1.5f, (Color){80, 80, 120, 200});

    // Column headers
    DrawTextEx(gameFont, "#", (Vector2){cardX + 50, cardY + 75}, 16.0f, 1.0f, (Color){150, 150, 170, 255});
    DrawTextEx(gameFont, "Name", (Vector2){cardX + 90, cardY + 75}, 16.0f, 1.0f, (Color){150, 150, 170, 255});
    DrawTextEx(gameFont, "Score", (Vector2){cardX + cardW - 80, cardY + 75}, 16.0f, 1.0f, (Color){150, 150, 170, 255});

    // Entries
    int entryY = cardY + 105;
    for (int i = 0; i < state->scoreboardCount && i < 10; i++) {
        char rankStr[8];
        sprintf(rankStr, "%d.", i + 1);
        Color entryColor = (i == 0) ? (Color){255, 220, 50, 255} : COLOR_TEXT_PRIMARY;

        DrawTextEx(gameFont, rankStr, (Vector2){cardX + 50, (float)entryY}, 18.0f, 1.0f, entryColor);
        DrawTextEx(gameFont, state->scoreboardNames[i], (Vector2){cardX + 90, (float)entryY}, 18.0f, 1.0f, entryColor);

        char scoreStr[16];
        sprintf(scoreStr, "%d", state->scoreboardScores[i]);
        int scoreW = (int)MeasureTextEx(gameFont, scoreStr, 18.0f, 1.0f).x;
        DrawTextEx(gameFont, scoreStr, (Vector2){cardX + cardW - 80 - scoreW, (float)entryY}, 18.0f, 1.0f, entryColor);

        entryY += 30;
    }

    if (state->scoreboardCount == 0) {
        DrawMenuTextCenteredX("No scores yet!", cardY + 150, 20, (Color){100, 100, 140, 200});
    }

    // Reset Scoreboard button
    int btnY = cardY + cardH - 140;
    Rectangle resetSBBtn = {BTN_X, btnY, BTN_W, BTN_H};
    bool resetSBHover = CheckCollisionPointRec(GetMousePosition(), resetSBBtn);
    DrawMenuButtonStyled(resetSBBtn, "Reset Scoreboard", 18,
                     (Color){140, 40, 40, 255}, (Color){180, 60, 60, 255},
                     (Color){160, 60, 60, 255}, (Color){220, 80, 80, 255},
                     (Color){220, 180, 180, 255}, (Color){255, 220, 220, 255},
                     resetSBHover);

    // Back button
    int backBtnY = btnY + BTN_H + BTN_GAP;
    Rectangle backBtn = {BTN_X, backBtnY, BTN_W, BTN_H};
    bool backHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawMenuButtonStyled(backBtn, "Back", 18,
                     (Color){40, 40, 60, 255}, (Color){60, 60, 80, 255},
                     (Color){80, 80, 100, 255}, (Color){150, 150, 180, 255},
                     (Color){180, 180, 210, 255}, (Color){255, 255, 255, 255},
                     backHover);

    DrawMenuTextCenteredX("ESC: Go Back", SCREEN_HEIGHT - 25, 14, (Color){100, 100, 140, 200});
}
