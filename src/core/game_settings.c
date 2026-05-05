#include "game.h"
#include "sound.h"

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
        Rectangle itemRect = { SCREEN_WIDTH / 2 - 120, 270 + i * 55 - 5, 240, 40 };
        if (CheckCollisionPointRec(mouse, itemRect)) {
            state->selectedSetting = i;
        }
    }

    // Helper macro for executing a setting action
    #define EXECUTE_SETTING(idx) do { \
        SoundPlayMenuClick(&state->sound); \
        switch (idx) { \
            case SETTING_SFX: SoundToggleSfx(&state->sound); break; \
            case SETTING_MUSIC: SoundToggleMusic(&state->sound); break; \
            case SETTING_RESTART: \
                if (state->prevScreen == SCREEN_ADVENTURE_PLAY) { \
                    state->currentScreen = SCREEN_ADVENTURE_PLAY; \
                    GameResetAdventure(state); \
                } else { \
                    state->currentScreen = SCREEN_PLAY; \
                    GameReset(state); \
                } \
                break; \
            case SETTING_QUIT: \
                if (state->prevScreen == SCREEN_ADVENTURE_PLAY) { \
                    state->currentScreen = SCREEN_ADVENTURE_MAP; \
                } else { \
                    state->currentScreen = SCREEN_MENU; \
                } \
                break; \
        } \
    } while(0)

    // Select setting with ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        EXECUTE_SETTING(state->selectedSetting);
    }

    // Click on setting items
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < SETTING_COUNT; i++) {
            Rectangle itemRect = { SCREEN_WIDTH / 2 - 120, 270 + i * 55 - 5, 240, 40 };
            if (CheckCollisionPointRec(mouse, itemRect)) {
                EXECUTE_SETTING(i);
                break;
            }
        }
    }

    #undef EXECUTE_SETTING

    // Back to game with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&state->sound);
        state->currentScreen = state->prevScreen;
    }
}