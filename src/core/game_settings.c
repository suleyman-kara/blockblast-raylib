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

void GameUpdateSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();

    // Keyboard navigation is removed in the new design since settings items
    // are now visual icons/buttons. We only handle mouse clicks.

    // Calculate positions matching RenderSettings() in render_menus.c
    const SettingsStyle *s = &THEME_DEFAULT.settings;
    int cardX = (SCREEN_WIDTH - s->cardWidth) / 2;
    int cardY = (SCREEN_HEIGHT - s->cardHeight) / 2 - 20;

    // SFX & Music icon areas
    int iconAreaY = cardY + 85;
    int iconSpacing = (s->cardWidth - 2 * s->paddingX) / 2;
    int iconStartX = cardX + s->paddingX;

    int sfxIconX = iconStartX + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
    Rectangle sfxRect = { (float)sfxIconX, (float)iconAreaY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE };

    int musicIconX = iconStartX + iconSpacing + (iconSpacing - SETTINGS_ICON_SIZE) / 2;
    Rectangle musicRect = { (float)musicIconX, (float)iconAreaY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE };

    // Replay and Home buttons
    int replayBtnY = iconAreaY + SETTINGS_ICON_SIZE + 40;
    int btnX = (SCREEN_WIDTH - SETTINGS_BTN_W) / 2;
    Rectangle replayRect = { (float)btnX, (float)replayBtnY, SETTINGS_BTN_W, SETTINGS_BTN_H };

    int homeBtnY = replayBtnY + SETTINGS_BTN_H + SETTINGS_BTN_GAP;
    Rectangle homeRect = { (float)btnX, (float)homeBtnY, SETTINGS_BTN_W, SETTINGS_BTN_H };

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