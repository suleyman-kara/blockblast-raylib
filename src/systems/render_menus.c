#include "render.h"
#include "font.h"
#include "theme.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

// Draw an infinity symbol (∞) at (cx, cy) with given radius and color
static void DrawInfinitySymbol(int cx, int cy, int r, Color color)
{
    int thickness = r / 2;
    if (thickness < 3) thickness = 3;
    DrawCircleLines(cx - r/2, cy, r/2, color);
    DrawCircleLines(cx + r/2, cy, r/2, color);
    DrawLine(cx - r/2, cy - r/2, cx + r/2, cy - r/2, color);
    DrawLine(cx - r/2, cy + r/2, cx + r/2, cy + r/2, color);
}

// Draw a simple hourglass icon at (cx, cy) with given size and color
static void DrawHourglass(int cx, int cy, int size, Color color)
{
    int half = size / 2;
    DrawTriangle((Vector2){cx - half, cy - half},
                 (Vector2){cx + half, cy - half},
                 (Vector2){cx, cy},
                 color);
    DrawTriangle((Vector2){cx - half, cy + half},
                 (Vector2){cx + half, cy + half},
                 (Vector2){cx, cy},
                 color);
}

// ----- Menu screen -----
void RenderMenu(GameState *state)
{
    const Theme *t = &THEME_DEFAULT;

    // Title — rainbow gradient per character, font size 72
    const char *title = "BLOCK BLAST";
    int titleLen = strlen(title);
    int titleFontSize = 72;

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

    Vector2 mouse = GetMousePosition();

    // --- Standard Mode Button ---
    Rectangle stdBtn = { MENU_BTN_X, MENU_STD_Y, MENU_BTN_W, MENU_BTN_H };
    bool stdHover = CheckCollisionPointRec(mouse, stdBtn);
    const ButtonStyle *stdStyle = &t->menuStandard;

    Color stdBg = stdHover ? stdStyle->bgHover : stdStyle->bg;
    Color stdBorder = stdHover ? stdStyle->borderHover : stdStyle->border;

    DrawRectangleRounded(stdBtn, stdStyle->cornerRadius, stdStyle->borderSegments, stdBg);
    DrawRectangleRoundedLines(stdBtn, stdStyle->cornerRadius, stdStyle->borderSegments, stdBorder);

    int iconY = MENU_STD_Y + MENU_BTN_H / 2;
    int iconX = MENU_BTN_X + 25;
    Color iconColor = stdHover ? (Color){180, 220, 255, 255} : (Color){130, 160, 200, 255};
    DrawInfinitySymbol(iconX, iconY, 16, iconColor);

    const char *stdText = "Classic Mode";
    int stw = GameMeasureText(stdText, 22);
    Color stdTextColor = stdHover ? stdStyle->textHover : stdStyle->text;
    GameDrawText(stdText, MENU_BTN_X + 55 + (MENU_BTN_W - 55 - stw) / 2,
                 MENU_STD_Y + (MENU_BTN_H - 22) / 2, 22, stdTextColor);

    // --- Adventure Mode Button ---
    Rectangle advBtn = { MENU_BTN_X, MENU_ADV_Y, MENU_BTN_W, MENU_BTN_H };
    bool advHover = CheckCollisionPointRec(mouse, advBtn);
    const ButtonStyle *advStyle = &t->menuAdventure;

    Color advBg = advHover ? advStyle->bgHover : advStyle->bg;
    Color advBorder = advHover ? advStyle->borderHover : advStyle->border;

    DrawRectangleRounded(advBtn, advStyle->cornerRadius, advStyle->borderSegments, advBg);
    DrawRectangleRoundedLines(advBtn, advStyle->cornerRadius, advStyle->borderSegments, advBorder);

    Color advIconColor = advHover ? (Color){220, 180, 255, 255} : (Color){160, 120, 200, 255};
    DrawHourglass(iconX, MENU_ADV_Y + MENU_BTN_H / 2, 22, advIconColor);

    const char *advText = "Adventure Mode";
    int atw = GameMeasureText(advText, 22);
    Color advTextColor = advHover ? advStyle->textHover : advStyle->text;
    GameDrawText(advText, MENU_BTN_X + 55 + (MENU_BTN_W - 55 - atw) / 2,
                 MENU_ADV_Y + (MENU_BTN_H - 22) / 2, 22, advTextColor);
}

// ----- Settings screen (overlay with framed card) -----
void RenderSettings(GameState *state)
{
    const SettingsStyle *s = &THEME_DEFAULT.settings;

    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, s->overlayBg);

    // Settings card frame
    int cardX = (SCREEN_WIDTH - s->cardWidth) / 2;
    int cardY = (SCREEN_HEIGHT - s->cardHeight) / 2 - 20;

    // Card background
    DrawRectangleRounded((Rectangle){cardX, cardY, s->cardWidth, s->cardHeight}, 0.15f, 8, s->cardBg);
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, s->cardWidth, s->cardHeight}, 0.15f, 8, s->cardBorder);

    // Title
    const char *title = "SETTINGS";
    int tw = GameMeasureText(title, s->titleFontSize);
    GameDrawText(title, (SCREEN_WIDTH - tw) / 2, cardY + 25, s->titleFontSize, WHITE);

    // Separator line
    DrawLineEx((Vector2){cardX + 40, cardY + 65},
               (Vector2){cardX + s->cardWidth - 40, cardY + 65},
               1.5f, s->separator);

    // Build settings labels dynamically based on sound state
    char sfxLabel[32];
    char musicLabel[32];
    sprintf(sfxLabel, "Sound FX: %s", state->sound.sfxEnabled ? "ON" : "OFF");
    sprintf(musicLabel, "Music: %s", state->sound.musicEnabled ? "ON" : "OFF");

    const char *items[SETTING_COUNT] = {
        sfxLabel,
        musicLabel,
        "Restart",
        "Main Menu"
    };

    for (int i = 0; i < SETTING_COUNT; i++) {
        int itemY = cardY + s->paddingTop + i * s->itemSpacing;

        if (i == state->selectedSetting) {
            // Selected item: golden with indicator
            const char *indicator = "> ";
            GameDrawText(indicator, cardX + 30, itemY, s->itemFontSize, s->selectedText);
            GameDrawText(items[i], cardX + 55, itemY, s->itemFontSize, s->selectedText);
        } else {
            GameDrawText(items[i], cardX + 55, itemY, s->itemFontSize, s->unselectedText);
        }
    }

    // Footer hint
    const char *hint = "ESC to go back";
    int hw = GameMeasureText(hint, 14);
    GameDrawText(hint, (SCREEN_WIDTH - hw) / 2, cardY + s->cardHeight - 30, 14, s->footerHint);
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