#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "core/defs.h"
#include "board.h"
#include "piece.h"
#include "anim.h"
#include "float_text.h"
#include "particle.h"
#include "sound.h"
#include "adventure.h"

#include <math.h>

// Global font loaded from a TrueType file (defined in main.c)
extern Font gameFont;

// Game screens
typedef enum {
    SCREEN_MENU,
    SCREEN_PLAY,
    SCREEN_SETTINGS,
    SCREEN_GAMEOVER,
    SCREEN_ADVENTURE_MAP,
    SCREEN_ADVENTURE_PLAY,
    SCREEN_ADVENTURE_RESULT
} Screen;

// "BEST COMBO!" banner animation
typedef struct {
    bool active;
    float timer;
    float duration;
    char text[32];
} BannerAnim;

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
    Vector2 dragStartPos; // position where drag started (for speed calculation)

    // Animation & effects
    AnimQueue anims;
    FloatTextQueue floatTexts;
    ParticleSystem particles;
    BannerAnim banner;

    // Settings menu
    int selectedSetting;

    // Sound system
    SoundSystem sound;

    bool gameOver;

    // Adventure mode
    AdventureState adventure;
    AdventureSaveData adventureSave;
    int selectedLevel;  // which level is selected on the map

    Screen prevScreen;  // screen before settings overlay
} GameState;

// Initialize a new game state
void GameInit(GameState *state);

// Update game logic for one frame
void GameUpdate(GameState *state);

// Reset board and slots for a new game (keeps highscore)
void GameReset(GameState *state);

// Reset board and slots for adventure mode (loads level obstacles)
void GameResetAdventure(GameState *state);

// Update settings screen logic (keyboard/mouse navigation)
void GameUpdateSettings(GameState *state);

#endif // GAME_H