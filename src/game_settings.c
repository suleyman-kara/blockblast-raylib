#include "game.h"
#include "sound.h"
#include "defs.h"

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

    // Calculate positions matching RenderSettings() in render_menus.c
    int cardX = (SCREEN_WIDTH - SETTINGS_CARD_WIDTH) / 2;
    int cardY = (SCREEN_HEIGHT - SETTINGS_CARD_HEIGHT) / 2 - 20;

    // SFX & Music icon areas
    int iconAreaY = cardY + 85;
    int iconSpacing = (SETTINGS_CARD_WIDTH - 2 * SETTINGS_CARD_PADDING_X) / 2;
    int iconStartX = cardX + SETTINGS_CARD_PADDING_X;

    int sfxIconX = iconStartX + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
    Rectangle sfxRect = { (float)sfxIconX, (float)iconAreaY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE };

    int musicIconX = iconStartX + iconSpacing + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
    Rectangle musicRect = { (float)musicIconX, (float)iconAreaY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE };

    // Replay and Home buttons
    int replayBtnY = iconAreaY + SETTINGS_ICON_SIZE + 40;
    Rectangle replayRect = { (float)BTN_X, (float)replayBtnY, BTN_W, BTN_H };

    int homeBtnY = replayBtnY + BTN_H + BTN_GAP;
    Rectangle homeRect = { (float)BTN_X, (float)homeBtnY, BTN_W, BTN_H };

    // Hover highlight for visual feedback
    if (CheckCollisionPointRec(mouse, sfxRect)) {
        state->selectedSetting = SETTING_SFX;
    } else if (CheckCollisionPointRec(mouse, musicRect)) {
        state->selectedSetting = SETTING_MUSIC;
    } else if (CheckCollisionPointRec(mouse, replayRect)) {
        state->selectedSetting = SETTING_RESTART;
    } else if (CheckCollisionPointRec(mouse, homeRect)) {
        state->selectedSetting = SETTING_QUIT;
    }

    // Click handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, sfxRect)) {
            ExecuteSetting(state, SETTING_SFX);
        } else if (CheckCollisionPointRec(mouse, musicRect)) {
            ExecuteSetting(state, SETTING_MUSIC);
        } else if (CheckCollisionPointRec(mouse, replayRect)) {
            ExecuteSetting(state, SETTING_RESTART);
        } else if (CheckCollisionPointRec(mouse, homeRect)) {
            ExecuteSetting(state, SETTING_QUIT);
        }
    }

    // Back to game with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&state->sound);
        state->currentScreen = state->prevScreen;
    }
}