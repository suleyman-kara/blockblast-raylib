#ifndef FONT_H
#define FONT_H

#include "raylib.h"

// Global font loaded from a TrueType file
extern Font gameFont;

// Load/unload the game font (call once at startup/shutdown)
void LoadGameFont(void);
void UnloadGameFont(void);

// Font-aware text helpers (replacement for blurry DrawText)
int GameMeasureText(const char *text, int fontSize);
void GameDrawText(const char *text, int posX, int posY, int fontSize, Color color);

#endif // FONT_H