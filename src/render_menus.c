#include "defs.h"
#include "render.h"
#include "textures.h"
#include <math.h>
#include <stdio.h>
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

  bool allCompleted = true;
  for (int i = 0; i < TOTAL_LEVELS; i++) {
      if (!state->levelCompleted[i]) {
          allCompleted = false;
          break;
      }
  }

  if (allCompleted) {
      const char *compText = "Completed";
      int ctw = (int)MeasureTextEx(gameFont, compText, 14.0f, 1.0f).x;
      DrawTextEx(gameFont, compText,
                 (Vector2){(float)(BTN_X + (BTN_W - ctw) / 2),
                           (float)(MENU_ADV_Y + BTN_H + 5)},
                 14.0f, 1.0f, (Color){ 50,  255, 100, 255 });
  }

  // --- Game Simulation Decoration ---
  int s = 35; // Block size for mini grid
  int startX = (SCREEN_WIDTH - (4 * s)) / 2;
  int startY = 580; // Slightly lower so the hovering piece fits

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

  // --- Debug Buttons (Complete and Reset) ---
  Rectangle dbgCompleteBtn = {10, SCREEN_HEIGHT - 40, 100, 30};
  Rectangle dbgResetBtn = {SCREEN_WIDTH - 110, SCREEN_HEIGHT - 40, 100, 30};

  bool hoverComplete = CheckCollisionPointRec(mouse, dbgCompleteBtn);
  bool hoverReset = CheckCollisionPointRec(mouse, dbgResetBtn);

  // Complete Button (Green)
  Color compColor = hoverComplete ? (Color){ 80, 220, 80, 255 } : (Color){ 40, 180, 40, 255 };
  DrawRectangleRec(dbgCompleteBtn, compColor);
  DrawRectangleLinesEx(dbgCompleteBtn, 1, DARKGRAY);
  int compW = MeasureText("Complete", 12);
  DrawText("Complete", dbgCompleteBtn.x + (dbgCompleteBtn.width - compW) / 2, dbgCompleteBtn.y + (dbgCompleteBtn.height - 12) / 2, 12, WHITE);

  // Reset Button (Red)
  Color resetColor = hoverReset ? (Color){ 240, 80, 80, 255 } : (Color){ 200, 40, 40, 255 };
  DrawRectangleRec(dbgResetBtn, resetColor);
  DrawRectangleLinesEx(dbgResetBtn, 1, DARKGRAY);
  int resetW = MeasureText("Reset", 12);
  DrawText("Reset", dbgResetBtn.x + (dbgResetBtn.width - resetW) / 2, dbgResetBtn.y + (dbgResetBtn.height - 12) / 2, 12, WHITE);
}

// ----- Settings screen (overlay with framed card) -----
void RenderSettings(GameState *state) {
  // Semi-transparent dark overlay
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SETTINGS_OVERLAY_BG);

  // Settings card frame
  int cardX = (SCREEN_WIDTH - SETTINGS_CARD_WIDTH) / 2;
  int cardY = (SCREEN_HEIGHT - SETTINGS_CARD_HEIGHT) / 2 - 20;

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
  int iconAreaY = cardY + 85;
  int iconSpacing = (SETTINGS_CARD_WIDTH - 2 * SETTINGS_CARD_PADDING_X) / 2;
  int iconStartX = cardX + SETTINGS_CARD_PADDING_X;

  // SFX icon
  int sfxIconX = iconStartX + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
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
  int musicIconX =
      iconStartX + iconSpacing + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
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
  int replayBtnY = iconAreaY + SETTINGS_ICON_SIZE + 40;
  Rectangle replayBtn = {(float)BTN_X, (float)replayBtnY, BTN_W, BTN_H};
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
  int homeBtnY = replayBtnY + BTN_H + BTN_GAP;
  Rectangle homeBtn = {(float)BTN_X, (float)homeBtnY, BTN_W, BTN_H};
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