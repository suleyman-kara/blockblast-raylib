#include "game.h"
#include "score.h"
#include "adventure.h"
#include "sound.h"

// Helper: get the rectangle for an adventure level button on the map
static Rectangle GetLevelButtonRect(int levelIndex)
{
    int btnSize = 80;
    int gap = 15;
    int totalRowWidth = LEVELS_PER_ROW * btnSize + (LEVELS_PER_ROW - 1) * gap;
    int mapStartX = (SCREEN_WIDTH - totalRowWidth) / 2;
    int mapStartY = 150;
    int row = levelIndex / LEVELS_PER_ROW;
    int col = levelIndex % LEVELS_PER_ROW;
    return (Rectangle){
        mapStartX + col * (btnSize + gap),
        mapStartY + row * (btnSize + gap + 30),
        btnSize,
        btnSize
    };
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

    Vector2 mouse = GetMousePosition();

    switch (state->currentScreen) {
        case SCREEN_MENU: {
            // Standard Mode button
            Rectangle stdBtn = { BTN_X, MENU_STD_Y, BTN_W, BTN_H };
            // Adventure Mode button
            Rectangle advBtn = { BTN_X, MENU_ADV_Y, BTN_W, BTN_H };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, stdBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_PLAY;
                    GameReset(state);
                } else if (CheckCollisionPointRec(mouse, advBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_ADVENTURE_MAP;
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

            const LevelDef *level = AdventureGetLevelDefs();
            level = &level[state->adventure.currentLevel];

            // Check if level is complete
            bool goalMet = false;
            switch (level->goalType) {
                case GOAL_SCORE:
                    goalMet = (state->score >= state->adventure.goalScore);
                    break;
                case GOAL_GEMS:
                    goalMet = AdventureCheckGoal(&state->adventure);
                    break;
                case GOAL_MIXED_GEMS:
                    goalMet = AdventureCheckGoal(&state->adventure);
                    break;
                case GOAL_MIXED_ALL:
                    goalMet = (state->score >= state->adventure.goalScore) &&
                              AdventureCheckGoal(&state->adventure);
                    break;
            }

            if (goalMet && !state->adventure.levelComplete) {
                state->adventure.levelComplete = true;
                state->adventure.showResultScreen = true;

                // Save progress
                int lvl = state->adventure.currentLevel;
                state->adventureSave.levels[lvl].completed = true;

                // Unlock next level
                if (lvl + 1 < TOTAL_LEVELS) {
                    state->adventureSave.levels[lvl + 1].unlocked = true;
                }

                AdventureSaveProgress(&state->adventureSave);

                // Go to result screen immediately
                state->currentScreen = SCREEN_ADVENTURE_RESULT;
            }

            // Check if level failed (no valid moves left)
            if (AdventureCheckFailure(&state->adventure, &state->board, state->slots) && !state->adventure.levelComplete) {
                state->adventure.levelFailed = true;
                state->adventure.levelComplete = true;
                state->adventure.showResultScreen = true;

                // Go to result screen immediately
                state->currentScreen = SCREEN_ADVENTURE_RESULT;
            }

            break;
        }

        case SCREEN_ADVENTURE_MAP: {
            // Leave icon (top-left)
            Rectangle leaveRect = { 15, 15, 32, 32 };

            // Level selection via clicking
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Check leave icon first
                if (CheckCollisionPointRec(mouse, leaveRect)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_MENU;
                    break;
                }

                for (int i = 0; i < TOTAL_LEVELS; i++) {
                    Rectangle btn = GetLevelButtonRect(i);

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
            // Result screen button definitions (must match render_adventure.c)
            const int cardH = 400;
            const int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
            const int topBtnY = cardY + cardH - 140;
            const int botBtnY = topBtnY + BTN_H + BTN_GAP;

            Rectangle btnTop = { BTN_X, topBtnY, BTN_W, BTN_H };
            Rectangle btnBot = { BTN_X, botBtnY, BTN_W, BTN_H };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (state->adventure.levelFailed) {
                    // Top: Tekrar Dene | Bottom: Ana Menü
                    if (CheckCollisionPointRec(mouse, btnTop)) {
                        // Tekrar Dene — restart same level
                        SoundPlayMenuClick(&state->sound);
                        state->adventure.showResultScreen = false;
                        state->adventure.levelFailed = false;
                        state->adventure.levelComplete = false;
                        GameResetAdventure(state);
                        state->currentScreen = SCREEN_ADVENTURE_PLAY;
                    } else if (CheckCollisionPointRec(mouse, btnBot)) {
                        // Ana Menü
                        SoundPlayMenuClick(&state->sound);
                        state->currentScreen = SCREEN_MENU;
                        state->adventure.showResultScreen = false;
                        state->adventure.levelFailed = false;
                        state->adventure.levelComplete = false;
                    }
                } else {
                    // Top: Sonraki Level | Bottom: Ana Menü
                    if (CheckCollisionPointRec(mouse, btnTop)) {
                        // Sonraki Level
                        SoundPlayMenuClick(&state->sound);
                        int nextLevel = state->adventure.currentLevel + 1;
                        state->adventure.showResultScreen = false;
                        state->adventure.levelComplete = false;
                        if (nextLevel < TOTAL_LEVELS) {
                            state->selectedLevel = nextLevel;
                            GameResetAdventure(state);
                            state->currentScreen = SCREEN_ADVENTURE_PLAY;
                        } else {
                            // All levels completed — go to menu
                            state->currentScreen = SCREEN_MENU;
                        }
                    } else if (CheckCollisionPointRec(mouse, btnBot)) {
                        // Ana Menü
                        SoundPlayMenuClick(&state->sound);
                        state->currentScreen = SCREEN_MENU;
                        state->adventure.showResultScreen = false;
                        state->adventure.levelComplete = false;
                    }
                }
            }

            // ESC to go back to adventure map
            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->adventure.showResultScreen = false;
                state->adventure.levelFailed = false;
                state->adventure.levelComplete = false;
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