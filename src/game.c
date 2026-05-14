#include "game.h"
#include "save.h"
#include "sound.h"
#include "ui_layout.h"
#include <ctype.h>
#include <string.h>


// ─── Game Init ───────────────────────────────────────────────────────────────
void GameInit(GameState *state)
{
    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    LevelLoadDefinitions();
    BoardInit(&state->board);
    state->highScore = ScoreLoadHigh();
    state->selectedLevel = 0;

    // Load nickname
    NicknameLoad(state->nickname, 32);
    state->nicknameInput[0] = '\0';
    state->nicknameCursorPos = 0;

    // Load scoreboard
    ScoreboardLoad(state->scoreboardScores, state->scoreboardNames, &state->scoreboardCount);

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        SlotClear(&state->slots[i]);

    state->unlockedLevel = LevelLoadProgress();
}

// ─── Game Reset (unified classic + adventure) ────────────────────────────────
void GameReset(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        SlotClear(&state->slots[i]);

    // Initialize level (sets up board + prefill)
    LevelInit(&state->level, state->selectedLevel, &state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;

    // Determine gem chances from level definition
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
    float diamondChance = (def->targetDiamonds > 0) ? DIAMOND_SPAWN_CHANCE : 0.0f;
    float emeraldChance = (def->targetEmeralds > 0) ? EMERALD_SPAWN_CHANCE : 0.0f;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance, emeraldChance);
}


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

                if (state->level.currentLevel == state->unlockedLevel &&
                    state->unlockedLevel <= TOTAL_LEVELS) {
                    state->unlockedLevel++;
                    LevelSaveProgress(state->unlockedLevel);
                }

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
                        if (LevelIsUnlocked(state->unlockedLevel, i + 1)) {
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
            // Result screen buttons (must match RenderResult layout in render_ui.c)
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
            state->unlockedLevel = 1;
            LevelSaveProgress(state->unlockedLevel);
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
