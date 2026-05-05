#include "font.h"
#include <stdio.h>

Font gameFont = { 0 };

void LoadGameFont(void)
{
    // Load Russo One — bold, modern game font
    // Use a high base size (96) so the glyph texture has enough resolution
    // for all font sizes we use (14..48). raylib will scale the texture when
    // drawing at different sizes; bilinear filtering makes this smooth.
    gameFont = LoadFontEx("assets/fonts/RussoOne-Regular.ttf", 200, 0, 0);
    if (gameFont.texture.id == 0) {
        fprintf(stderr, "WARNING: Failed to load Russo One, trying Fredoka...\n");
        gameFont = LoadFontEx("assets/fonts/Fredoka-Variable.ttf", 96, 0, 0);
    }
    if (gameFont.texture.id == 0) {
        fprintf(stderr, "WARNING: Failed to load Fredoka, trying Bebas Neue...\n");
        gameFont = LoadFontEx("assets/fonts/BebasNeue-Regular.ttf", 96, 0, 0);
    }
    if (gameFont.texture.id == 0) {
        fprintf(stderr, "WARNING: Failed to load custom fonts, falling back to default.\n");
        gameFont = GetFontDefault();
    } else {
        // Set bilinear filtering on the font texture so scaling is smooth
        SetTextureFilter(gameFont.texture, TEXTURE_FILTER_BILINEAR);
        printf("INFO: FONT: Loaded custom font successfully.\n");
    }
}

void UnloadGameFont(void)
{
    if (gameFont.texture.id != 0) {
        UnloadFont(gameFont);
    }
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