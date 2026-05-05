#include "render.h"
#include "font.h"
#include <stdio.h>

// ----- Menu screen -----
void RenderMenu(GameState *state)
{
    // Title
    const char *title = "BLOCK BLAST";
    int tw = GameMeasureText(title, 48);
    GameDrawText(title, (SCREEN_WIDTH - tw)/2, 180, 48, WHITE);

    // High score
    char buf[64];
    sprintf(buf, "En Yuksek Skor: %d", state->highScore);
    int hw = GameMeasureText(buf, 20);
    GameDrawText(buf, (SCREEN_WIDTH - hw)/2, 260, 20,
        (Color){100,100,130,255});

    // Mouse position for hover effects
    Vector2 mouse = GetMousePosition();

    // --- Standard Mode Button ---
    Rectangle stdBtn = { MENU_BTN_X, MENU_STD_Y, MENU_BTN_W, MENU_BTN_H };
    bool stdHover = CheckCollisionPointRec(mouse, stdBtn);
    Color stdBg = stdHover ? (Color){60, 60, 100, 255} : (Color){45, 45, 75, 255};
    Color stdBorder = stdHover ? (Color){100, 150, 255, 255} : (Color){70, 70, 110, 255};

    // Button background with rounded look
    DrawRectangleRounded(stdBtn, 0.3f, 8, stdBg);
    DrawRectangleRoundedLines(stdBtn, 0.3f, 8, stdBorder);

    const char *stdText = "Standart Mod";
    int stw = GameMeasureText(stdText, 24);
    GameDrawText(stdText, (SCREEN_WIDTH - stw) / 2, MENU_STD_Y + (MENU_BTN_H - 24) / 2, 24,
        stdHover ? WHITE : (Color){200, 200, 220, 255});

    // --- Adventure Mode Button ---
    Rectangle advBtn = { MENU_BTN_X, MENU_ADV_Y, MENU_BTN_W, MENU_BTN_H };
    bool advHover = CheckCollisionPointRec(mouse, advBtn);
    Color advBg = advHover ? (Color){60, 50, 80, 255} : (Color){40, 35, 60, 255};
    Color advBorder = advHover ? (Color){180, 100, 255, 255} : (Color){80, 60, 110, 255};

    DrawRectangleRounded(advBtn, 0.3f, 8, advBg);
    DrawRectangleRoundedLines(advBtn, 0.3f, 8, advBorder);

    const char *advText = "Macera Modu";
    int atw = GameMeasureText(advText, 24);
    GameDrawText(advText, (SCREEN_WIDTH - atw) / 2, MENU_ADV_Y + (MENU_BTN_H - 24) / 2, 24,
        advHover ? (Color){220, 200, 255, 255} : (Color){150, 130, 180, 255});
}

// ----- Settings screen (overlay) -----
void RenderSettings(GameState *state)
{
    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

    // Title
    const char *title = "AYARLAR";
    int tw = GameMeasureText(title, 36);
    GameDrawText(title, (SCREEN_WIDTH - tw) / 2, 200, 36, WHITE);

    // Separator line
    DrawRectangle(SCREEN_WIDTH / 2 - 80, 245, 160, 2, (Color){80, 80, 100, 255});

    // Build settings labels dynamically based on sound state
    char sfxLabel[32];
    char musicLabel[32];
    sprintf(sfxLabel, "Ses Efektleri: %s", state->sound.sfxEnabled ? "ACIK" : "KAPALI");
    sprintf(musicLabel, "Muzik: %s", state->sound.musicEnabled ? "ACIK" : "KAPALI");

    const char *items[SETTING_COUNT] = {
        sfxLabel,
        musicLabel,
        "Yeniden Baslat",
        "Ana Menuye Don"
    };

    for (int i = 0; i < SETTING_COUNT; i++) {
        int fontSize = 24;
        Color clr;

        if (i == state->selectedSetting) {
            // Selected item: golden with indicator
            clr = (Color){255, 220, 50, 255};
            const char *indicator = "> ";
            int fullW = GameMeasureText(indicator, fontSize) + GameMeasureText(items[i], fontSize);
            int x = (SCREEN_WIDTH - fullW) / 2;
            int y = 270 + i * 55;
            GameDrawText(indicator, x, y, fontSize, clr);
            GameDrawText(items[i], x + GameMeasureText(indicator, fontSize), y, fontSize, clr);
        } else {
            clr = (Color){180, 180, 200, 255};
            int w = GameMeasureText(items[i], fontSize);
            GameDrawText(items[i], (SCREEN_WIDTH - w) / 2, 270 + i * 55, fontSize, clr);
        }
    }

    // Footer hint
    const char *hint = "ESC: Geri Don  |  Tikla veya ENTER: Sec";
    int hw = GameMeasureText(hint, 16);
    GameDrawText(hint, (SCREEN_WIDTH - hw) / 2, 510, 16, (Color){100, 100, 120, 255});
}

// ----- Game Over screen -----
void RenderGameOver(GameState *state)
{
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,160});

    const char *go = "OYUN BITTI";
    int gw = GameMeasureText(go, 44);
    GameDrawText(go, (SCREEN_WIDTH - gw)/2, 280, 44, (Color){255,87,87,255});

    char buf[64];
    sprintf(buf, "Skor: %d", state->score);
    int sw = GameMeasureText(buf, 28);
    GameDrawText(buf, (SCREEN_WIDTH - sw)/2, 350, 28, WHITE);

    sprintf(buf, "En Yuksek: %d", state->highScore);
    sw = GameMeasureText(buf, 22);
    GameDrawText(buf, (SCREEN_WIDTH - sw)/2, 395, 22, (Color){150,150,170,255});

    const char *retry = "Tekrar oynamak icin ENTER'a bas";
    int rw = GameMeasureText(retry, 18);
    GameDrawText(retry, (SCREEN_WIDTH - rw)/2, 460, 18,
        (Color){100,100,130,255});
}