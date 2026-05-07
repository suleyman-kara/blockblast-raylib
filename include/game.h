#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "defs.h"
#include "board.h"
#include "piece.h"
#include "anim.h"
#include "float_text.h"
#include "particle.h"
#include "sound.h"
#include "level.h"

#include <math.h>

// Global font loaded from a TrueType file (defined in main.c)
extern Font gameFont;

// Game screens
typedef enum {
    SCREEN_MENU,
    SCREEN_PLAY,          // unified classic + adventure
    SCREEN_LEVEL_SELECT,
    SCREEN_RESULT,        // adaptive win/lose
    SCREEN_SETTINGS
} Screen;

// Central game state — passed by pointer to all modules
typedef struct {
    Screen currentScreen;
    Board board;
    PieceSlot slots[3];
    int score;
    int highScore;
    int combo;

    // Drag state
    bool isDragging;
    int dragSlotIndex;
    Vector2 dragPos;      // current drag position (screen coords)
    Vector2 dragOffset;   // offset from piece origin to mouse

    // Animation & effects
    AnimQueue anims;
    FloatTextQueue floatTexts;
    ParticleSystem particles;

    // Settings menu
    int selectedSetting;

    // Sound system
    SoundSystem sound;

    // Level
    LevelState level;
    int selectedLevel;              // 0 = classic, 1-10 = adventure
    bool levelCompleted[TOTAL_LEVELS]; // which adventure levels are completed (persisted)

    Screen prevScreen;  // screen before settings overlay
} GameState;

// Initialize a new game state
void GameInit(GameState *state);

// Update game logic for one frame
void GameUpdate(GameState *state);

// Reset board and slots for a new game (uses selectedLevel to determine behavior)
void GameReset(GameState *state);

// Update settings screen logic (keyboard/mouse navigation)
void GameUpdateSettings(GameState *state);

// High score save/load (moved from deleted score module)
int ScoreLoadHigh(void);
void ScoreSaveHigh(int score);

#endif // GAME_H