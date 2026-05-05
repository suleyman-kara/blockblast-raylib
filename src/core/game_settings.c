#include "game.h"
#include "sound.h"
#include "theme.h"

// Execute a setting action for the given index
static void ExecuteSetting(GameState *state, int index)
{
    SoundPlayMenuClick(&state->sound);
    switch (index) {
        case SETTING_SFX:
            SoundToggleSfx(&state->sound);
            break;
        case SETTING_MUSIC:
            SoundToggleMusic(&state->sound);
            break;
        case SETTING_RESTART:
            if (state->prevScreen == SCREEN_ADVENTURE_PLAY) {
                state->currentScreen = SCREEN_ADVENTURE_PLAY;
                GameResetAdventure(state);
            } else {
                state->currentScreen = SCREEN_PLAY;
                GameReset(state);
            }
            break;
        case SETTING_QUIT:
            if (state->prevScreen == SCREEN_ADVENTURE_PLAY) {
                state->currentScreen = SCREEN_ADVENTURE_MAP;
            } else {
                state->currentScreen = SCREEN_MENU;
            }
            break;
    }
}

// Calculate the card's position and item rects - used both for render and hit test
static Rectangle GetSettingsItemRect(int index)
{
    const SettingsStyle *s = &THEME_DEFAULT.settings;
    int cardX = (SCREEN_WIDTH - s->cardWidth) / 2;
    int cardY = (SCREEN_HEIGHT - s->cardHeight) / 2 - 20;
    int itemY = cardY + s->paddingTop + index * s->itemSpacing;
    return (Rectangle){ cardX + 30, itemY - 5, (float)(s->cardWidth - 60), 40 };
}

void GameUpdateSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();

    // Navigate settings with keyboard
    if (IsKeyPressed(KEY_UP)) {
        state->selectedSetting--;
        if (state->selectedSetting < 0)
            state->selectedSetting = SETTING_COUNT - 1;
        SoundPlayMenuClick(&state->sound);
    }
    if (IsKeyPressed(KEY_DOWN)) {
        state->selectedSetting++;
        if (state->selectedSetting >= SETTING_COUNT)
            state->selectedSetting = 0;
        SoundPlayMenuClick(&state->sound);
    }

    // Hover highlight: update selectedSetting based on mouse position
    for (int i = 0; i < SETTING_COUNT; i++) {
        Rectangle itemRect = GetSettingsItemRect(i);
        if (CheckCollisionPointRec(mouse, itemRect)) {
            state->selectedSetting = i;
        }
    }

    // Select setting with ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        ExecuteSetting(state, state->selectedSetting);
    }

    // Click on setting items
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < SETTING_COUNT; i++) {
            Rectangle itemRect = GetSettingsItemRect(i);
            if (CheckCollisionPointRec(mouse, itemRect)) {
                ExecuteSetting(state, i);
                break;
            }
        }
    }

    // Back to game with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&state->sound);
        state->currentScreen = state->prevScreen;
    }
}