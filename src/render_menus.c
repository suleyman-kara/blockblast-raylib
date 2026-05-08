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
  // Title — rainbow gradient per character, font size TITLE_FONT_SIZE
  const char *title = "BLOCK BLAST";
  int titleLen = strlen(title);
  int titleFontSize = TITLE_FONT_SIZE;

  float titleTotalW = 0;
  for (int i = 0; i < titleLen; i++) {
    char ch[2] = {title[i], '\0'};
    titleTotalW += MeasureTextEx(gameFont, ch, (float)titleFontSize, 1.0f).x;
  }
  float titleX = (SCREEN_WIDTH - titleTotalW) / 2.0f;
  float cursorX = titleX;
  for (int i = 0; i < titleLen; i++) {
    char ch[2] = {title[i], '\0'};
    float hue = (i / (float)titleLen) * 300.0f;
    Color c = ColorFromHSV(hue, 0.6f, 0.9f);
    Vector2 sz = MeasureTextEx(gameFont, ch, (float)titleFontSize, 1.0f);
    DrawTextEx(gameFont, ch, (Vector2){cursorX, 150}, (float)titleFontSize,
               1.0f, c);
    cursorX += sz.x;
  }

  // Subtitle "Adventure Master" — white with black stroke, 50% of title size
  const char *subtitle = "Adventure Master";
  int subFontSize = SUBTITLE_FONT_SIZE;
  int subW = (int)MeasureTextEx(gameFont, subtitle, (float)subFontSize, 1.0f).x;
  int subX = (SCREEN_WIDTH - subW) / 2;
  int subY = 150 + titleFontSize + 10;
  // Black stroke (draw 4 times offset)
  DrawTextEx(gameFont, subtitle, (Vector2){(float)(subX - 2), (float)subY},
             (float)subFontSize, 1.0f, BLACK);
  DrawTextEx(gameFont, subtitle, (Vector2){(float)(subX + 2), (float)subY},
             (float)subFontSize, 1.0f, BLACK);
  DrawTextEx(gameFont, subtitle, (Vector2){(float)subX, (float)(subY - 2)},
             (float)subFontSize, 1.0f, BLACK);
  DrawTextEx(gameFont, subtitle, (Vector2){(float)subX, (float)(subY + 2)},
             (float)subFontSize, 1.0f, BLACK);
  // White text on top
  DrawTextEx(gameFont, subtitle, (Vector2){(float)subX, (float)subY},
             (float)subFontSize, 1.0f, WHITE);

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