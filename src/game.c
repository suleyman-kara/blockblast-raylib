#include "game.h"
#include "score.h"
#include <string.h>

// Setting items for the settings screen
#define SETTING_COUNT 4
#define SETTING_SFX     0
#define SETTING_MUSIC   1
#define SETTING_RESTART 2
#define SETTING_QUIT    3

void GameInit(GameState *state)
{
    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    BoardInit(&state->board);
    state->highScore = ScoreLoadHigh();
    state->combo = 0;
    state->score = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;
    state->selectedSetting = 0;

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        state->slots[i].piece = NULL;
}

void GameReset(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state->slots[i].piece);

    BoardInit(&state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);
}

void GameUpdate(GameState *state)
{
    float dt = GetFrameTime();

    // Always update visual effects (they should animate on all screens)
    FloatTextUpdate(&state->floatTexts, dt);
    ParticleUpdate(&state->particles, dt);

    // Banner timer
    if (state->banner.active) {
        state->banner.timer -= dt;
        if (state->banner.timer <= 0.0f) {
            state->banner.active = false;
        }
    }

    switch (state->currentScreen) {
        case SCREEN_MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_PLAY;
                GameReset(state);
            }
            break;

        case SCREEN_PLAY:
            // Update animations
            AnimUpdate(&state->anims, dt);

            // Open settings with ESC
            if (IsKeyPressed(KEY_ESCAPE)) {
                state->currentScreen = SCREEN_SETTINGS;
                state->selectedSetting = 0;
            }

            // Check game over transition
            if (state->gameOver) {
                SoundPlayLose(&state->sound);
                state->currentScreen = SCREEN_GAMEOVER;
            }
            break;

        case SCREEN_SETTINGS:
            // Navigate settings
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

            // Select setting
            if (IsKeyPressed(KEY_ENTER)) {
                SoundPlayMenuClick(&state->sound);
                switch (state->selectedSetting) {
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
                        state->currentScreen = SCREEN_MENU;
                        break;
                }
            }

            // Back to game with ESC
            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_PLAY;
            }
            break;

        case SCREEN_GAMEOVER:
            if (IsKeyPressed(KEY_ENTER)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_PLAY;
                GameReset(state);
            }
            break;
    }
}
