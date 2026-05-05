#include "game.h"
#include "score.h"
#include "adventure.h"
#include "sound.h"

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

    Vector2 mouse = GetMousePosition();

    switch (state->currentScreen) {
        case SCREEN_MENU: {
            // Standard Mode button
            Rectangle stdBtn = { MENU_BTN_X, MENU_STD_Y, MENU_BTN_W, MENU_BTN_H };
            // Adventure Mode button
            Rectangle advBtn = { MENU_BTN_X, MENU_ADV_Y, MENU_BTN_W, MENU_BTN_H };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, stdBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_PLAY;
                    GameReset(state);
                } else if (CheckCollisionPointRec(mouse, advBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    // Check lives
                    AdventureRegenLives(&state->adventureSave);
                    if (state->adventureSave.currentLives <= 0) {
                        // No lives - show message (handled in render)
                        state->currentScreen = SCREEN_ADVENTURE_MAP;
                    } else {
                        state->currentScreen = SCREEN_ADVENTURE_MAP;
                    }
                }
            }
            break;
        }

        case SCREEN_PLAY: {
            // Update animations
            AnimUpdate(&state->anims, dt);

            // Open settings by clicking the gear icon (top-right)
            Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(mouse, gearRect)) {
                state->prevScreen = state->currentScreen;
                state->currentScreen = SCREEN_SETTINGS;
                state->selectedSetting = 0;
            }

            // Check game over transition
            if (state->gameOver) {
                SoundPlayLose(&state->sound);
                state->currentScreen = SCREEN_GAMEOVER;
            }
            break;
        }

        case SCREEN_ADVENTURE_PLAY: {
            // Update animations
            AnimUpdate(&state->anims, dt);

            // Open settings by clicking the gear icon (top-right)
            Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(mouse, gearRect)) {
                state->prevScreen = state->currentScreen;
                state->currentScreen = SCREEN_SETTINGS;
                state->selectedSetting = 0;
            }

            // Check if level is complete (goal reached)
            if (AdventureCheckGoal(&state->adventure) && !state->adventure.levelComplete) {
                state->adventure.levelComplete = true;
                state->adventure.earnedStars = AdventureCalcStars(&state->adventure);
                state->adventure.showResultScreen = true;
                state->adventure.resultTimer = 2.0f;

                // Save progress
                int lvl = state->adventure.currentLevel;
                if (state->adventure.earnedStars > state->adventureSave.levels[lvl].bestStars)
                    state->adventureSave.levels[lvl].bestStars = state->adventure.earnedStars;
                state->adventureSave.levels[lvl].completed = true;

                // Unlock next level
                if (lvl + 1 < TOTAL_LEVELS) {
                    state->adventureSave.levels[lvl + 1].unlocked = true;
                }

                AdventureSaveProgress(&state->adventureSave);
            }

            // Check if level failed (pieces exhausted without reaching goal)
            if (AdventureCheckFailure(&state->adventure) && !state->adventure.levelComplete) {
                state->adventure.levelFailed = true;
                state->adventure.levelComplete = true;
                state->adventure.earnedStars = 0;
                state->adventure.showResultScreen = true;
                state->adventure.resultTimer = 2.0f;

                // Lose a life
                state->adventureSave.currentLives--;
                AdventureSaveLives(&state->adventureSave);
            }

            // Result screen timer
            if (state->adventure.showResultScreen) {
                state->adventure.resultTimer -= dt;
                if (state->adventure.resultTimer <= 0.0f) {
                    state->currentScreen = SCREEN_ADVENTURE_MAP;
                    state->adventure.showResultScreen = false;
                }
            }

            break;
        }

        case SCREEN_ADVENTURE_MAP: {
            // Level selection via clicking
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Calculate level button positions (same as render)
                int mapStartY = 200;
                int btnSize = 80;
                int gap = 15;
                int totalRowWidth = LEVELS_PER_ROW * btnSize + (LEVELS_PER_ROW - 1) * gap;
                int mapStartX = (SCREEN_WIDTH - totalRowWidth) / 2;

                for (int i = 0; i < TOTAL_LEVELS; i++) {
                    int row = i / LEVELS_PER_ROW;
                    int col = i % LEVELS_PER_ROW;
                    Rectangle btn = {
                        mapStartX + col * (btnSize + gap),
                        mapStartY + row * (btnSize + gap + 30),
                        btnSize,
                        btnSize
                    };

                    if (CheckCollisionPointRec(mouse, btn)) {
                        if (state->adventureSave.levels[i].unlocked) {
                            SoundPlayMenuClick(&state->sound);
                            state->selectedLevel = i;
                            state->currentScreen = SCREEN_ADVENTURE_PLAY;
                            GameResetAdventure(state);
                        }
                        break;
                    }
                }
            }

            // ESC to go back to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_MENU;
            }
            break;
        }

        case SCREEN_ADVENTURE_RESULT: {
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_ADVENTURE_MAP;
            }
            break;
        }

        case SCREEN_SETTINGS:
            GameUpdateSettings(state);
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
