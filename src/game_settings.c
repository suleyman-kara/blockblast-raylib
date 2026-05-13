#include "game.h"
#include "sound.h"
#include "defs.h"
#include "ui_layout.h"
#include <string.h>

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
            state->currentScreen = SCREEN_PLAY;
            GameReset(state);
            break;
        case SETTING_QUIT:
            // Use selectedLevel to decide where Home goes
            state->currentScreen = (state->selectedLevel > 0) ? SCREEN_LEVEL_SELECT : SCREEN_MENU;
            break;
    }
}

void GameUpdateSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();
    SettingsLayout layout = GetSettingsLayout(false);

    // Hover highlight for visual feedback
    if (CheckCollisionPointRec(mouse, layout.sfxIcon)) {
        state->selectedSetting = SETTING_SFX;
    } else if (CheckCollisionPointRec(mouse, layout.musicIcon)) {
        state->selectedSetting = SETTING_MUSIC;
    } else if (CheckCollisionPointRec(mouse, layout.firstButton)) {
        state->selectedSetting = SETTING_RESTART;
    } else if (CheckCollisionPointRec(mouse, layout.secondButton)) {
        state->selectedSetting = SETTING_QUIT;
    }

    // Click handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, layout.sfxIcon)) {
            ExecuteSetting(state, SETTING_SFX);
        } else if (CheckCollisionPointRec(mouse, layout.musicIcon)) {
            ExecuteSetting(state, SETTING_MUSIC);
        } else if (CheckCollisionPointRec(mouse, layout.firstButton)) {
            ExecuteSetting(state, SETTING_RESTART);
        } else if (CheckCollisionPointRec(mouse, layout.secondButton)) {
            ExecuteSetting(state, SETTING_QUIT);
        }
    }

    // Back to game with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&state->sound);
        state->currentScreen = state->prevScreen;
    }
}

// Menu settings: SFX, Music, Change Nickname, Home (no Replay)
void GameUpdateMenuSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();
    SettingsLayout layout = GetSettingsLayout(true);

    // Click handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, layout.sfxIcon)) {
            SoundPlayMenuClick(&state->sound);
            SoundToggleSfx(&state->sound);
        } else if (CheckCollisionPointRec(mouse, layout.musicIcon)) {
            SoundPlayMenuClick(&state->sound);
            SoundToggleMusic(&state->sound);
        } else if (CheckCollisionPointRec(mouse, layout.firstButton)) {
            SoundPlayMenuClick(&state->sound);
            strcpy(state->nicknameInput, state->nickname);
            state->nicknameCursorPos = strlen(state->nicknameInput);
            state->prevScreen = SCREEN_MENU_SETTINGS;
            state->currentScreen = SCREEN_NICKNAME;
        } else if (CheckCollisionPointRec(mouse, layout.secondButton)) {
            SoundPlayMenuClick(&state->sound);
            // Reset level progress
            for (int i = 0; i < TOTAL_LEVELS; i++) {
                state->levelCompleted[i] = false;
            }
            LevelSaveProgress(state->levelCompleted);
            // Reset classic mode high score
            state->highScore = 0;
            ScoreSaveHigh(state->highScore);
        } else if (CheckCollisionPointRec(mouse, layout.thirdButton)) {
            SoundPlayMenuClick(&state->sound);
            state->currentScreen = SCREEN_MENU;
        }
    }

    // Back to menu with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&state->sound);
        state->currentScreen = SCREEN_MENU;
    }
}
