#include "font.h"
#include <stdio.h>

Font gameFont = { 0 };

void LoadGameFont(void)
{
    gameFont = LoadFontEx("assets/fonts/RussoOne-Regular.ttf", 200, 0, 0);
    if (gameFont.texture.id != 0)
        printf("INFO: FONT: Loaded custom font successfully.\n");
}

void UnloadGameFont(void)
{
    if (gameFont.texture.id != 0)  UnloadFont(gameFont);
}

int GameMeasureText(const char *text, int fontSize)
{
    Vector2 sz = MeasureTextEx(gameFont, text, (float)fontSize, 1.0f);
    return (int)sz.x;
}

void GameDrawText(const char *text, int posX, int posY, int fontSize, Color color)
{
    DrawTextEx(gameFont, text, (Vector2){(float)posX, (float)posY},
               (float)fontSize, 1.0f, color);
}