#include "game.h"
#include "level.h"
#include "sound.h"
#include <string.h>
#include <ctype.h>

// Helper: get the rectangle for a level button on the level select screen
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

    // Always update visual effects
    FloatTextUpdate(&state->floatTexts, dt);
    ParticleUpdate(&state->particles, dt);

    Vector2 mouse = GetMousePosition();

    switch (state->currentScreen) {
        case SCREEN_MENU: {
            Rectangle stdBtn = { BTN_X, MENU_STD_Y, BTN_W, BTN_H };
            Rectangle advBtn = { BTN_X, MENU_ADV_Y, BTN_W, BTN_H };
            Rectangle quitBtn = { BTN_X, MENU_QUIT_Y, BTN_W, BTN_H };
            Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
            Rectangle sbBtn = {10, SCREEN_HEIGHT - 40, 100, 30};

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, stdBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->selectedLevel = 0;
                    // Check if nickname is set
                    if (strlen(state->nickname) == 0) {
                        // No nickname yet, ask for one
                        strcpy(state->nicknameInput, "");
                        state->nicknameCursorPos = 0;
                        state->prevScreen = SCREEN_MENU;
                        state->currentScreen = SCREEN_NICKNAME;
                    } else {
                        state->currentScreen = SCREEN_PLAY;
                        GameReset(state);
                    }
                } else if (CheckCollisionPointRec(mouse, advBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_LEVEL_SELECT;
                } else if (CheckCollisionPointRec(mouse, quitBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    CloseWindow();
                } else if (CheckCollisionPointRec(mouse, gearRect)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_MENU_SETTINGS;
                } else if (CheckCollisionPointRec(mouse, sbBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_SCOREBOARD;
                }
            }
            break;
        }

        case SCREEN_PLAY: {
            // Update animations
            AnimUpdate(&state->anims, dt);

            // Open settings by clicking the gear icon
            Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(mouse, gearRect)) {
                state->prevScreen = state->currentScreen;
                state->currentScreen = SCREEN_SETTINGS;
                state->selectedSetting = 0;
            }

            // Check if level goals are met (only for adventure, level > 0)
            if (state->selectedLevel > 0 &&
                LevelCheckGoal(&state->level, state->score) &&
                !state->level.levelComplete) {
                state->level.levelComplete = true;

                // Save progress
                int lvl = state->level.currentLevel;
                state->levelCompleted[lvl - 1] = true;  // levelCompleted is 0-indexed for levels 1-10
                LevelSaveProgress(state->levelCompleted);

                state->currentScreen = SCREEN_RESULT;
            }

            // Check if no valid moves (game over / level failed)
            if (LevelCheckFailure(&state->board, state->slots) &&
                !state->level.levelComplete) {
                state->level.levelFailed = true;

                if (state->selectedLevel == 0) {
                    // Classic mode: save high score and add to scoreboard
                    if (state->score > state->highScore)
                        state->highScore = state->score;
                    ScoreSaveHigh(state->highScore);

                    // Add to scoreboard if nickname is set
                    if (strlen(state->nickname) > 0) {
                        ScoreboardAddEntry(state->scoreboardScores, state->scoreboardNames,
                                           &state->scoreboardCount, state->nickname, state->score);
                    }

                    SoundPlayLose(&state->sound);
                }

                state->currentScreen = SCREEN_RESULT;
            }
            break;
        }

        case SCREEN_LEVEL_SELECT: {
            // Leave icon (top-left)
            Rectangle leaveRect = { 15, 15, 32, 32 };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, leaveRect)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_MENU;
                    break;
                }

                for (int i = 0; i < TOTAL_LEVELS; i++) {
                    Rectangle btn = GetLevelButtonRect(i);
                    if (CheckCollisionPointRec(mouse, btn)) {
                        if (LevelIsUnlocked(state->levelCompleted, i + 1)) {
                            SoundPlayMenuClick(&state->sound);
                            state->selectedLevel = i + 1;  // levels are 1-indexed
                            state->currentScreen = SCREEN_PLAY;
                            GameReset(state);
                        }
                        break;
                    }
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_MENU;
            }
            break;
        }

        case SCREEN_RESULT: {
            // Result screen buttons (must match render_level.c layout)
            const int cardH = 400;
            const int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
            const int topBtnY = cardY + cardH - 140;
            const int botBtnY = topBtnY + BTN_H + BTN_GAP;

            Rectangle btnTop = { BTN_X, topBtnY, BTN_W, BTN_H };
            Rectangle btnBot = { BTN_X, botBtnY, BTN_W, BTN_H };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (state->level.levelFailed) {
                    // Failed: Top = Retry, Bottom = Home
                    if (CheckCollisionPointRec(mouse, btnTop)) {
                        SoundPlayMenuClick(&state->sound);
                        state->level.levelFailed = false;
                        state->level.levelComplete = false;
                        GameReset(state);
                        state->currentScreen = SCREEN_PLAY;
                    } else if (CheckCollisionPointRec(mouse, btnBot)) {
                        SoundPlayMenuClick(&state->sound);
                        state->level.levelFailed = false;
                        state->level.levelComplete = false;
                        state->currentScreen = (state->selectedLevel > 0) ? SCREEN_LEVEL_SELECT : SCREEN_MENU;
                    }
                } else {
                    // Won: Top = Next Level, Bottom = Home
                    if (CheckCollisionPointRec(mouse, btnTop)) {
                        SoundPlayMenuClick(&state->sound);
                        int nextLevel = state->level.currentLevel + 1;
                        state->level.levelComplete = false;
                        if (nextLevel <= TOTAL_LEVELS) {
                            state->selectedLevel = nextLevel;
                            GameReset(state);
                            state->currentScreen = SCREEN_PLAY;
                        } else {
                            state->currentScreen = SCREEN_LEVEL_SELECT;
                        }
                    } else if (CheckCollisionPointRec(mouse, btnBot)) {
                        SoundPlayMenuClick(&state->sound);
                        state->level.levelComplete = false;
                        state->currentScreen = SCREEN_LEVEL_SELECT;
                    }
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->level.levelFailed = false;
                state->level.levelComplete = false;
                state->currentScreen = (state->selectedLevel > 0) ? SCREEN_LEVEL_SELECT : SCREEN_MENU;
            }
            break;
        }

        case SCREEN_SETTINGS:
            GameUpdateSettings(state);
            break;

        case SCREEN_MENU_SETTINGS:
            GameUpdateMenuSettings(state);
            break;

        case SCREEN_NICKNAME: {
            // Handle keyboard input for nickname
            int key = GetCharPressed();
            while (key > 0) {
                // Only allow printable ASCII characters
                if (key >= 32 && key <= 126) {
                    int len = strlen(state->nicknameInput);
                    if (len < 30) {
                        state->nicknameInput[len] = (char)key;
                        state->nicknameInput[len + 1] = '\0';
                    }
                }
                key = GetCharPressed();
            }

            // Backspace
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(state->nicknameInput);
                if (len > 0) {
                    state->nicknameInput[len - 1] = '\0';
                }
            }

            // Confirm with Enter
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                if (strlen(state->nicknameInput) > 0) {
                    SoundPlayMenuClick(&state->sound);
                    strncpy(state->nickname, state->nicknameInput, 31);
                    state->nickname[31] = '\0';
                    NicknameSave(state->nickname);
                    if (state->prevScreen == SCREEN_MENU && state->selectedLevel == 0) {
                        state->currentScreen = SCREEN_PLAY;
                        GameReset(state);
                    } else {
                        state->currentScreen = state->prevScreen;
                    }
                }
            }

            // Confirm with mouse click on button
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int inputH = 50;
                int cardH = 300;
                int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
                int inputY = cardY + 100;
                Rectangle confirmBtn = { BTN_X, inputY + inputH + 30, BTN_W, BTN_H };

                if (CheckCollisionPointRec(mouse, confirmBtn) && strlen(state->nicknameInput) > 0) {
                    SoundPlayMenuClick(&state->sound);
                    strncpy(state->nickname, state->nicknameInput, 31);
                    state->nickname[31] = '\0';
                    NicknameSave(state->nickname);
                    if (state->prevScreen == SCREEN_MENU && state->selectedLevel == 0) {
                        state->currentScreen = SCREEN_PLAY;
                        GameReset(state);
                    } else {
                        state->currentScreen = state->prevScreen;
                    }
                }
            }

            // ESC to go back to previous screen
            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = state->prevScreen;
            }
            break;
        }

        case SCREEN_SCOREBOARD: {
            int cardH = 480;
            int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
            int btnY = cardY + cardH - 120;
            Rectangle resetSBBtn = { BTN_X, btnY, BTN_W, BTN_H };
            Rectangle backBtn = { BTN_X, btnY + BTN_H + BTN_GAP, BTN_W, BTN_H };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, resetSBBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->scoreboardCount = 0;
                    for (int i = 0; i < 10; i++) {
                        state->scoreboardScores[i] = 0;
                        state->scoreboardNames[i][0] = '\0';
                    }
                    ScoreboardSave(state->scoreboardScores, state->scoreboardNames, 0);
                } else if (CheckCollisionPointRec(mouse, backBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_MENU;
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_MENU;
            }
            break;
        }
    }
}
