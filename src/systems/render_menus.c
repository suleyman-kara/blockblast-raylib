#include "render.h"
#include "font.h"
#include "defs.h"
#include "textures.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

// ----- Menu screen -----
void RenderMenu(GameState *state)
{
    // Title — rainbow gradient per character, font size TITLE_FONT_SIZE
    const char *title = "BLOCK BLAST";
    int titleLen = strlen(title);
    int titleFontSize = TITLE_FONT_SIZE;

    float titleTotalW = 0;
    for (int i = 0; i < titleLen; i++) {
        char ch[2] = { title[i], '\0' };
        titleTotalW += MeasureTextEx(gameFont, ch, (float)titleFontSize, 1.0f).x;
    }
    float titleX = (SCREEN_WIDTH - titleTotalW) / 2.0f;
    float cursorX = titleX;
    for (int i = 0; i < titleLen; i++) {
        char ch[2] = { title[i], '\0' };
        float hue = (i / (float)titleLen) * 300.0f;
        Color c = ColorFromHSV(hue, 0.6f, 0.9f);
        Vector2 sz = MeasureTextEx(gameFont, ch, (float)titleFontSize, 1.0f);
        DrawTextEx(gameFont, ch, (Vector2){ cursorX, 150 }, (float)titleFontSize, 1.0f, c);
        cursorX += sz.x;
    }

    // Subtitle "Adventure Master" — white with black stroke, 50% of title size
    const char *subtitle = "Adventure Master";
    int subFontSize = SUBTITLE_FONT_SIZE;
    int subW = GameMeasureText(subtitle, subFontSize);
    int subX = (SCREEN_WIDTH - subW) / 2;
    int subY = 150 + titleFontSize + 10;
    // Black stroke (draw 4 times offset)
    GameDrawText(subtitle, subX - 2, subY, subFontSize, BLACK);
    GameDrawText(subtitle, subX + 2, subY, subFontSize, BLACK);
    GameDrawText(subtitle, subX, subY - 2, subFontSize, BLACK);
    GameDrawText(subtitle, subX, subY + 2, subFontSize, BLACK);
    // White text on top
    GameDrawText(subtitle, subX, subY, subFontSize, WHITE);

    Vector2 mouse = GetMousePosition();

    // --- Standard Mode Button ---
    Rectangle stdBtn = { MENU_BTN_X, MENU_STD_Y, MENU_BTN_W, MENU_BTN_H };
    bool stdHover = CheckCollisionPointRec(mouse, stdBtn);

    Color stdBg = stdHover ? COLOR_BTN_STD_BG_HOVER : COLOR_BTN_STD_BG;
    Color stdBorder = stdHover ? COLOR_BTN_STD_BORDER_HOVER : COLOR_BTN_STD_BORDER;

    DrawRectangleRounded(stdBtn, BTN_STD_CORNER_RADIUS, BTN_STD_BORDER_SEGMENTS, stdBg);
    DrawRectangleRoundedLines(stdBtn, BTN_STD_CORNER_RADIUS, BTN_STD_BORDER_SEGMENTS, stdBorder);

    const char *stdText = "Classic Mode";
    int stw = GameMeasureText(stdText, 22);
    Color stdTextColor = stdHover ? COLOR_BTN_STD_TEXT_HOVER : COLOR_BTN_STD_TEXT;
    GameDrawText(stdText, MENU_BTN_X + (MENU_BTN_W - stw) / 2,
                 MENU_STD_Y + (MENU_BTN_H - 22) / 2, 22, stdTextColor);

    // --- Adventure Mode Button ---
    Rectangle advBtn = { MENU_BTN_X, MENU_ADV_Y, MENU_BTN_W, MENU_BTN_H };
    bool advHover = CheckCollisionPointRec(mouse, advBtn);

    Color advBg = advHover ? COLOR_BTN_ADV_BG_HOVER : COLOR_BTN_ADV_BG;
    Color advBorder = advHover ? COLOR_BTN_ADV_BORDER_HOVER : COLOR_BTN_ADV_BORDER;

    DrawRectangleRounded(advBtn, BTN_ADV_CORNER_RADIUS, BTN_ADV_BORDER_SEGMENTS, advBg);
    DrawRectangleRoundedLines(advBtn, BTN_ADV_CORNER_RADIUS, BTN_ADV_BORDER_SEGMENTS, advBorder);

    const char *advText = "Adventure Mode";
    int atw = GameMeasureText(advText, 22);
    Color advTextColor = advHover ? COLOR_BTN_ADV_TEXT_HOVER : COLOR_BTN_ADV_TEXT;
    GameDrawText(advText, MENU_BTN_X + (MENU_BTN_W - atw) / 2,
                 MENU_ADV_Y + (MENU_BTN_H - 22) / 2, 22, advTextColor);
}

// ----- Settings screen (overlay with framed card) -----
void RenderSettings(GameState *state)
{
    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SETTINGS_OVERLAY_BG);

    // Settings card frame
    int cardX = (SCREEN_WIDTH - SETTINGS_CARD_WIDTH) / 2;
    int cardY = (SCREEN_HEIGHT - SETTINGS_CARD_HEIGHT) / 2 - 20;

    // Card background
    DrawRectangleRounded((Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT}, 0.15f, 8, COLOR_SETTINGS_CARD_BG);
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT}, 0.15f, 8, COLOR_SETTINGS_CARD_BORDER);

    // Title
    const char *title = "SETTINGS";
    int tw = GameMeasureText(title, SETTINGS_TITLE_FONT_SIZE);
    GameDrawText(title, (SCREEN_WIDTH - tw) / 2, cardY + 25, SETTINGS_TITLE_FONT_SIZE, WHITE);

    // Separator line
    DrawLineEx((Vector2){cardX + 40, cardY + 65},
               (Vector2){cardX + SETTINGS_CARD_WIDTH - 40, cardY + 65},
               1.5f, COLOR_SETTINGS_SEPARATOR);


    Vector2 mouse = GetMousePosition();

    // ─── SFX & Music Icons (horizontal) ──────────────────────────────────────
    int iconAreaY = cardY + 85;
    int iconSpacing = (SETTINGS_CARD_WIDTH - 2 * SETTINGS_CARD_PADDING_X) / 2;
    int iconStartX = cardX + SETTINGS_CARD_PADDING_X;

    // SFX icon
    int sfxIconX = iconStartX + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
    DrawTexturePro(gameTextures.waveSound,
        (Rectangle){ 0, 0, (float)gameTextures.waveSound.width, (float)gameTextures.waveSound.height },
        (Rectangle){ (float)sfxIconX, (float)iconAreaY, (float)SETTINGS_ICON_SIZE, (float)SETTINGS_ICON_SIZE },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    // Red cross overlay if SFX is off (single diagonal: bottom-left to top-right)
    if (!state->sound.sfxEnabled) {
        int cx = sfxIconX;
        int cy = iconAreaY;
        DrawLineEx((Vector2){cx + 2, cy + SETTINGS_ICON_SIZE - 2}, (Vector2){cx + SETTINGS_ICON_SIZE - 2, cy + 2}, 5.0f, RED);
    }

    // SFX label below icon
    const char *sfxLabel = "SFX";
    int sfxLabelW = GameMeasureText(sfxLabel, 14);
    GameDrawText(sfxLabel, sfxIconX + (SETTINGS_ICON_SIZE - sfxLabelW) / 2,
                 iconAreaY + SETTINGS_ICON_SIZE + 4, 14, COLOR_SETTINGS_UNSELECTED_TEXT);

    // Music icon
    int musicIconX = iconStartX + iconSpacing + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
    DrawTexturePro(gameTextures.musicalNote,
        (Rectangle){ 0, 0, (float)gameTextures.musicalNote.width, (float)gameTextures.musicalNote.height },
        (Rectangle){ (float)musicIconX, (float)iconAreaY, (float)SETTINGS_ICON_SIZE, (float)SETTINGS_ICON_SIZE },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    // Red cross overlay if Music is off (single diagonal: bottom-left to top-right)
    if (!state->sound.musicEnabled) {
        int cx = musicIconX;
        int cy = iconAreaY;
        DrawLineEx((Vector2){cx + 2, cy + SETTINGS_ICON_SIZE - 2}, (Vector2){cx + SETTINGS_ICON_SIZE - 2, cy + 2}, 5.0f, RED);
    }

    // Music label below icon
    const char *musicLabel = "Music";
    int musicLabelW = GameMeasureText(musicLabel, 14);
    GameDrawText(musicLabel, musicIconX + (SETTINGS_ICON_SIZE - musicLabelW) / 2,
                 iconAreaY + SETTINGS_ICON_SIZE + 4, 14, COLOR_SETTINGS_UNSELECTED_TEXT);

    // ─── Replay Button ────────────────────────────────────────────────────────
    int replayBtnY = iconAreaY + SETTINGS_ICON_SIZE + 40;
    int btnX = (SCREEN_WIDTH - SETTINGS_BTN_W) / 2;
    Rectangle replayBtn = { (float)btnX, (float)replayBtnY, SETTINGS_BTN_W, SETTINGS_BTN_H };
    bool replayHover = CheckCollisionPointRec(mouse, replayBtn);

    Color replayBg = replayHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255};
    Color replayBorder = replayHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255};
    DrawRectangleRounded(replayBtn, 0.2f, 6, replayBg);
    DrawRectangleRoundedLines(replayBtn, 0.2f, 6, replayBorder);

    // Replay icon at left of text
    int replayIconSize = 20;
    DrawTexturePro(gameTextures.replay,
        (Rectangle){ 0, 0, (float)gameTextures.replay.width, (float)gameTextures.replay.height },
        (Rectangle){ (float)(btnX + 15), (float)(replayBtnY + (SETTINGS_BTN_H - replayIconSize) / 2), (float)replayIconSize, (float)replayIconSize },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    const char *replayText = "Replay";
    int replayTextW = GameMeasureText(replayText, 18);
    GameDrawText(replayText, btnX + 15 + replayIconSize + 8 + (SETTINGS_BTN_W - 15 - replayIconSize - 8 - replayTextW) / 2,
                 replayBtnY + (SETTINGS_BTN_H - 18) / 2, 18, COLOR_SETTINGS_UNSELECTED_TEXT);

    // ─── Home Button ──────────────────────────────────────────────────────────
    int homeBtnY = replayBtnY + SETTINGS_BTN_H + SETTINGS_BTN_GAP;
    Rectangle homeBtn = { (float)btnX, (float)homeBtnY, SETTINGS_BTN_W, SETTINGS_BTN_H };
    bool homeHover = CheckCollisionPointRec(mouse, homeBtn);

    Color homeBg = homeHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255};
    Color homeBorder = homeHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255};
    DrawRectangleRounded(homeBtn, 0.2f, 6, homeBg);
    DrawRectangleRoundedLines(homeBtn, 0.2f, 6, homeBorder);

    // Home icon at left of text
    int homeIconSize = 20;
    DrawTexturePro(gameTextures.home,
        (Rectangle){ 0, 0, (float)gameTextures.home.width, (float)gameTextures.home.height },
        (Rectangle){ (float)(btnX + 15), (float)(homeBtnY + (SETTINGS_BTN_H - homeIconSize) / 2), (float)homeIconSize, (float)homeIconSize },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    const char *homeText = "Home";
    int homeTextW = GameMeasureText(homeText, 18);
    GameDrawText(homeText, btnX + 15 + homeIconSize + 8 + (SETTINGS_BTN_W - 15 - homeIconSize - 8 - homeTextW) / 2,
                 homeBtnY + (SETTINGS_BTN_H - 18) / 2, 18, COLOR_SETTINGS_UNSELECTED_TEXT);

    // Footer hint
    const char *hint = "ESC to go back";
    int hw = GameMeasureText(hint, 14);
    GameDrawText(hint, (SCREEN_WIDTH - hw) / 2, cardY + SETTINGS_CARD_HEIGHT - 30, 14, COLOR_SETTINGS_FOOTER_HINT);
}

// ----- Game Over screen -----
void RenderGameOver(GameState *state)
{
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,160});

    const char *go = "GAME OVER";
    int gw = GameMeasureText(go, 44);
    GameDrawText(go, (SCREEN_WIDTH - gw)/2, 280, 44, (Color){255,87,87,255});

    char buf[64];
    sprintf(buf, "Score: %d", state->score);
    int sw = GameMeasureText(buf, 28);
    GameDrawText(buf, (SCREEN_WIDTH - sw)/2, 350, 28, WHITE);

    sprintf(buf, "Best: %d", state->highScore);
    sw = GameMeasureText(buf, 22);
    GameDrawText(buf, (SCREEN_WIDTH - sw)/2, 395, 22, (Color){150,150,170,255});

    const char *retry = "Press ENTER to play again";
    int rw = GameMeasureText(retry, 18);
    GameDrawText(retry, (SCREEN_WIDTH - rw)/2, 460, 18,
        (Color){100,100,130,255});
}