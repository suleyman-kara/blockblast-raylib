#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════════════════════════
//  BLOCK BLAST — Single file with all constants and colours
//  ═══════════════════════════════════════════════════════════════════════════════

// ─── Screen & Layout
// ──────────────────────────────────────────────────────────
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 800
#define CELL_SIZE 50
#define GRID_DRAW_X 40
#define GRID_DRAW_Y 130
#define PANEL_Y 580

// ─── Grid
// ─────────────────────────────────────────────────────────────────────
#define GRID_SIZE 8
#define CELL_EMPTY 0
#define GEM_NONE 0
#define GEM_DIAMOND 1
#define GEM_EMERALD 2

// ─── Piece System
// ─────────────────────────────────────────────────────────────
#define MAX_PIECE_SIZE 5
#define PIECE_DEF_COUNT 18
#define PANEL_PIECE_SCALE 25

// ─── Scoring
// ──────────────────────────────────────────────────────────────────
#define BASE_SCORE 100
#define ScoreCalculate(lines, combo) (BASE_SCORE * (lines) * (combo))

// ─── Animation
// ────────────────────────────────────────────────────────────────
#define CLEAR_ANIM_DURATION 0.35f
#define MAX_ANIMS 16

// ─── Floating Text
// ────────────────────────────────────────────────────────────
#define MAX_FLOAT_TEXTS 8
#define FLOAT_TEXT_LEN 32
#define FLOAT_TEXT_DURATION 1.2f
#define FLOAT_TEXT_SPEED 60.0f

// ─── Particles
// ────────────────────────────────────────────────────────────────
#define MAX_PARTICLES 512
#define PARTICLE_LIFE 0.7f

// ─── Adventure Mode
// ───────────────────────────────────────────────────────────
#define TOTAL_LEVELS 10
#define LEVELS_PER_ROW 3
#define DIAMOND_SPAWN_CHANCE 0.15f
#define EMERALD_SPAWN_CHANCE 0.12f

// ─── Settings Menu
// ────────────────────────────────────────────────────────────
#define SETTING_SFX 0
#define SETTING_MUSIC 1
#define SETTING_RESTART 2
#define SETTING_QUIT 3
#define SETTINGS_ICON_SIZE 40

// ─── Buttons (tüm ekranlarda aynı boyut)
// ──────────────────────────────────────
#define BTN_W 220
#define BTN_H 50
#define BTN_GAP 15
#define BTN_X ((SCREEN_WIDTH - BTN_W) / 2)
#define BTN_CORNER_RADIUS 0.25f
#define BTN_BORDER_SEGMENTS 8

// ─── Gear Icon
// ────────────────────────────────────────────────────────────────
#define GEAR_X (SCREEN_WIDTH - 50)
#define GEAR_Y 10
#define GEAR_SIZE 36

// ─── Menu Buttons (Y pozisyonları)
// ────────────────────────────────────────────
#define MENU_STD_Y 330
#define MENU_ADV_Y 400
#define MENU_QUIT_Y 470

// ─── Title & Subtitle
// ─────────────────────────────────────────────────────────
#define TITLE_FONT_SIZE 65
#define SUBTITLE_FONT_SIZE 32

// ═══════════════════════════════════════════════════════════════════════════════
//  Colour palette — flat constants, no structs
//  ═══════════════════════════════════════════════════════════════════════════════

// Background colour for the whole screen
#define COLOR_BG ((Color){40, 60, 100, 255})
#define BG_COLOR COLOR_BG // backward compatibility

// ─── Piece colours (index 0 = empty, 1-7 = piece colours) ────────────────────
const Color PIECE_COLORS[] = {
    {40, 40, 60, 255},   // 0 = empty cell
    {255, 87, 87, 255},  // 1 = red
    {255, 189, 46, 255}, // 2 = yellow
    {46, 204, 113, 255}, // 3 = green
    {52, 152, 219, 255}, // 4 = blue
    {155, 89, 182, 255}, // 5 = purple
    {255, 147, 51, 255}, // 6 = orange
    {26, 188, 156, 255}, // 7 = teal
};

// ─── Text
// ─────────────────────────────────────────────────────────────────────
#define COLOR_TEXT_PRIMARY ((Color){255, 255, 255, 255})
#define COLOR_TEXT_SECONDARY ((Color){150, 150, 170, 255})
#define COLOR_TEXT_MUTED ((Color){100, 100, 140, 200})

// ─── Settings overlay
// ─────────────────────────────────────────────────────────
#define SETTINGS_CARD_WIDTH 320
#define SETTINGS_CARD_HEIGHT 430

// ─── Gems
// ─────────────────────────────────────────────────────────────────────
#define GEM_SIZE_RATIO 0.50f

// ─── Adventure Map UI Constants ───────────────────────────────────────────────
#define COLOR_AMAP_LOCKED_BG       ((Color){ 25,  25,  40,  255 })
#define COLOR_AMAP_LOCKED_BORDER   ((Color){ 40,  40,  55,  255 })
#define COLOR_AMAP_UNLOCKED_BG     ((Color){ 40,  35,  60,  255 })
#define COLOR_AMAP_UNLOCKED_HOVER  ((Color){ 60,  50,  80,  255 })
#define COLOR_AMAP_UNLOCKED_BORDER ((Color){ 120, 80,  180, 255 })
#define COLOR_AMAP_COMPLETED_TEXT  ((Color){ 50,  255, 100, 255 })
#define COLOR_AMAP_LOCKED_NUMBER   ((Color){ 60,  60,  80,  255 })
#define AMAP_BTN_SIZE              80

// ─── Settings overlay colors
// ──────────────────────────────────────────────────
#define COLOR_SETTINGS_OVERLAY_BG ((Color){0, 0, 0, 160})
#define COLOR_SETTINGS_CARD_BG ((Color){20, 25, 50, 235})
#define COLOR_SETTINGS_CARD_BORDER ((Color){60, 70, 110, 180})
#define COLOR_SETTINGS_SEPARATOR ((Color){80, 80, 120, 200})
#define COLOR_SETTINGS_SELECTED_TEXT ((Color){255, 220, 50, 255})
#define COLOR_SETTINGS_UNSELECTED_TEXT ((Color){180, 180, 210, 255})
#define COLOR_SETTINGS_FOOTER_HINT ((Color){100, 100, 140, 200})

// ─── Menu Buttons — Classic Mode colors
// ───────────────────────────────────────
#define COLOR_BTN_STD_BG ((Color){60, 100, 180, 255})
#define COLOR_BTN_STD_BG_HOVER ((Color){80, 130, 210, 255})
#define COLOR_BTN_STD_BORDER ((Color){100, 150, 220, 255})
#define COLOR_BTN_STD_BORDER_HOVER ((Color){140, 200, 255, 255})
#define COLOR_BTN_STD_TEXT ((Color){220, 230, 250, 255})
#define COLOR_BTN_STD_TEXT_HOVER ((Color){255, 255, 255, 255})

// ─── Menu Buttons — Adventure Mode colors
// ─────────────────────────────────────
#define COLOR_BTN_ADV_BG ((Color){55, 40, 85, 255})
#define COLOR_BTN_ADV_BG_HOVER ((Color){80, 60, 120, 255})
#define COLOR_BTN_ADV_BORDER ((Color){100, 70, 150, 255})
#define COLOR_BTN_ADV_BORDER_HOVER ((Color){180, 120, 255, 255})
#define COLOR_BTN_ADV_TEXT ((Color){170, 150, 200, 255})
#define COLOR_BTN_ADV_TEXT_HOVER ((Color){230, 210, 255, 255})

// ─── Menu Buttons — Quit colors
// ──────────────────────────────
#define COLOR_BTN_QUIT_BG ((Color){140, 40, 40, 255})
#define COLOR_BTN_QUIT_BG_HOVER ((Color){180, 60, 60, 255})
#define COLOR_BTN_QUIT_BORDER ((Color){160, 60, 60, 255})
#define COLOR_BTN_QUIT_BORDER_HOVER ((Color){220, 80, 80, 255})
#define COLOR_BTN_QUIT_TEXT ((Color){220, 180, 180, 255})
#define COLOR_BTN_QUIT_TEXT_HOVER ((Color){255, 220, 220, 255})

// ─── Grid & Board Colors ────────────────────────────────────────────────────────
#define COLOR_GRID_BORDER       ((Color){ 50,  50,  70, 255 })
#define COLOR_GRID_EMPTY        ((Color){ 40,  40,  60, 255 })
#define COLOR_GRID_HIGHLIGHT    ((Color){ 255, 255, 255, 40  })

#define COLOR_PANEL_BG          ((Color){ 30,  30,  50, 255 })
#define COLOR_PANEL_TOP_BORDER  ((Color){ 50,  50,  70, 255 })
#define PANEL_TOP_BORDER_WIDTH  2.0f


/* ---- Piece types ---- */

// A block piece with its shape matrix and metadata
typedef struct {
    int shape[MAX_PIECE_SIZE][MAX_PIECE_SIZE]; // 1 = filled, 0 = empty
    int width;
    int height;
    int colorIndex; // 1-7 maps to a color in the palette
    int gemCells[MAX_PIECE_SIZE][MAX_PIECE_SIZE]; // GEM_NONE, GEM_DIAMOND, GEM_EMERALD
} Piece;

// A slot in the bottom panel holding one piece.
// occupied=false means the player already used this slot.
typedef struct {
    Piece piece;
    bool occupied;
    float posX;      // screen X for drawing in bottom panel
    float posY;      // screen Y for drawing in bottom panel
} PieceSlot;

// Static piece shape definition (used as templates)
typedef struct {
    int shape[MAX_PIECE_SIZE][MAX_PIECE_SIZE];
    int width;
    int height;
} PieceDef;

// ---- All piece shape definitions ----
const PieceDef PIECE_DEFS[PIECE_DEF_COUNT] = {
    // 1x1
    { .shape = {{1}}, .width = 1, .height = 1 },
    // 1x2 horizontal
    { .shape = {{1,1}}, .width = 2, .height = 1 },
    // 2x1 vertical
    { .shape = {{1},{1}}, .width = 1, .height = 2 },
    // 1x3 horizontal
    { .shape = {{1,1,1}}, .width = 3, .height = 1 },
    // 3x1 vertical
    { .shape = {{1},{1},{1}}, .width = 1, .height = 3 },
    // 1x4 horizontal
    { .shape = {{1,1,1,1}}, .width = 4, .height = 1 },
    // 4x1 vertical
    { .shape = {{1},{1},{1},{1}}, .width = 1, .height = 4 },
    // 1x5 horizontal
    { .shape = {{1,1,1,1,1}}, .width = 5, .height = 1 },
    // 5x1 vertical
    { .shape = {{1},{1},{1},{1},{1}}, .width = 1, .height = 5 },
    // 2x2 square
    { .shape = {{1,1},{1,1}}, .width = 2, .height = 2 },
    // 3x3 square
    { .shape = {{1,1,1},{1,1,1},{1,1,1}}, .width = 3, .height = 3 },
    // L-shape
    { .shape = {{1,0},{1,0},{1,1}}, .width = 2, .height = 3 },
    // Reverse L
    { .shape = {{0,1},{0,1},{1,1}}, .width = 2, .height = 3 },
    // L rotated
    { .shape = {{1,1,1},{1,0,0}}, .width = 3, .height = 2 },
    // Reverse L rotated
    { .shape = {{1,1,1},{0,0,1}}, .width = 3, .height = 2 },
    // T-shape
    { .shape = {{1,1,1},{0,1,0}}, .width = 3, .height = 2 },
    // Z-shape
    { .shape = {{1,1,0},{0,1,1}}, .width = 3, .height = 2 },
    // S-shape
    { .shape = {{0,1,1},{1,1,0}}, .width = 3, .height = 2 },
};


/* ---- Board types ---- */

// Board: 8x8 grid where blocks are placed
typedef struct {
  int cells[GRID_SIZE]
           [GRID_SIZE]; // 0 = empty, 1-7 = color index, 8=ice, 9=stone
  int gems[GRID_SIZE][GRID_SIZE]; // GEM_NONE, GEM_DIAMOND, GEM_EMERALD
} Board;

/* ---- Effect types ---- */

typedef struct {
    bool active;
    float timer;
    int row;
    int col;
} ClearAnim;

typedef struct {
    ClearAnim items[MAX_ANIMS * GRID_SIZE];
    int count;
} AnimQueue;

typedef struct {
    bool active;
    Vector2 pos;
    Vector2 vel;
    float life;
    float maxLife;
    float size;
    Color color;
    float gravity;
} Particle;

typedef struct {
    Particle items[MAX_PARTICLES];
    int count;
} ParticleSystem;

typedef struct {
    bool active;
    char text[FLOAT_TEXT_LEN];
    float x, y;
    float startY;
    float timer;
    float duration;
    float speed;
    int fontSize;
    Color color;
} FloatText;

typedef struct {
    FloatText items[MAX_FLOAT_TEXTS];
    int count;
} FloatTextQueue;

/* ---- Asset types ---- */

typedef struct {
    Font font;

    Texture2D crown;
    Texture2D diamond;
    Texture2D emerald;
    Texture2D home;
    Texture2D lock;
    Texture2D logout;
    Texture2D musicalNote;
    Texture2D waveSound;
    Texture2D replay;
    Texture2D completed;
    Texture2D logo;

    Sound sfxPlace;
    Sound sfxLineClear1;
    Sound sfxLineClear2;
    Sound sfxCombo1;
    Sound sfxCombo2;
    Sound sfxLose;
    Sound sfxMenuClick;
    Music bgMusic;

    bool sfxEnabled;
    bool musicEnabled;
} GameAssets;

GameAssets assets;

/* ---- Level types ---- */

// ─── Level Definition (immutable, const array) ────────────────────────────────
// Level 0 = classic (infinite play, all targets 0)
// Levels 1-10 = adventure levels
typedef struct {
    int level;
    int targetScore;      // 0 = no score target
    int targetDiamonds;   // 0 = no diamond target
    int targetEmeralds;   // 0 = no emerald target
    int prefillCount;     // random pieces to pre-place on board
} LevelDef;

// ─── Level State (mutable, runtime) ──────────────────────────────────────────
typedef struct {
    int currentLevel;         // index into level defs (0 = classic)
    int collectedDiamonds;
    int collectedEmeralds;
    bool levelComplete;
    bool levelFailed;
} LevelState;

// ─── Level Definitions ────────────────────────────────────────────────────────
// Index 0 = classic mode (infinite, no targets)
// Index 1-10 = adventure levels
const LevelDef defaultLevelDefs[TOTAL_LEVELS + 1] = {
    // Classic mode — no targets, no prefill
    { .level = 0,  .targetScore = 0, .targetDiamonds = 0, .targetEmeralds = 0, .prefillCount = 0 },

    // Adventure levels
    { .level = 1,  .targetScore = 500,  .prefillCount = 0 },
    { .level = 2,  .targetScore = 1000, .prefillCount = 0 },
    { .level = 3,  .targetDiamonds = 3, .prefillCount = 2 },
    { .level = 4,  .targetDiamonds = 5, .prefillCount = 3 },
    { .level = 5,  .targetDiamonds = 3, .targetEmeralds = 2, .prefillCount = 4 },
    { .level = 6,  .targetDiamonds = 4, .targetEmeralds = 3, .prefillCount = 5 },
    { .level = 7,  .targetScore = 500,  .targetDiamonds = 3, .targetEmeralds = 2, .prefillCount = 5 },
    { .level = 8,  .targetScore = 800,  .targetDiamonds = 4, .targetEmeralds = 3, .prefillCount = 6 },
    { .level = 9,  .targetScore = 1000, .targetDiamonds = 5, .targetEmeralds = 4, .prefillCount = 7 },
    { .level = 10, .targetScore = 1500, .targetDiamonds = 6, .targetEmeralds = 5, .prefillCount = 8 },
};

bool LevelIsUnlocked(int unlockedLevel, int lvl) {
    return (lvl >= 1 && lvl <= TOTAL_LEVELS && lvl <= unlockedLevel);
}

bool LevelIsCompleted(int unlockedLevel, int lvl) {
    return (lvl >= 1 && lvl <= TOTAL_LEVELS && lvl < unlockedLevel);
}

/* ---- Game state ---- */

// Game screens
typedef enum {
    SCREEN_MENU,
    SCREEN_PLAY,          // unified classic + adventure
    SCREEN_LEVEL_SELECT,
    SCREEN_RESULT,        // adaptive win/lose
    SCREEN_SETTINGS,
    SCREEN_MENU_SETTINGS  // settings from main menu
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

    // Level
    LevelState level;
    int selectedLevel;              // 0 = classic, 1-10 = adventure
    int unlockedLevel;              // 1 = first adventure level, TOTAL_LEVELS + 1 = all completed

    bool shouldQuit;
    Screen prevScreen;  // screen before settings overlay
} GameState;


/* ---- Function prototypes ---- */

const PieceDef *GetPieceDefinitions(void);
void PieceGenerate(Piece *out, float diamondChance, float emeraldChance);
void SlotClear(PieceSlot *slot);
bool SlotIsOccupied(const PieceSlot *slot);
void GenerateRandomPieces(PieceSlot slots[3], float panelY, float screenWidth,
                          float diamondChance, float emeraldChance);
bool AllSlotsEmpty(PieceSlot slots[3]);

void BoardInit(Board *board);
bool BoardCanPlace(Board *board, Piece *piece, int row, int col);
void BoardPlace(Board *board, Piece *piece, int row, int col);
int BoardClearLines(Board *board, bool clearedCells[GRID_SIZE][GRID_SIZE]);
bool BoardHasValidMove(Board *board, PieceSlot slots[3]);
void BoardPrefillGems(Board *board, int count, bool allowDiamonds,
                      bool allowEmeralds);

void AnimAddCleared(AnimQueue *queue, bool clearedCells[GRID_SIZE][GRID_SIZE]);
bool AnimUpdate(AnimQueue *queue, float dt);
bool AnimIsActive(AnimQueue *queue);
float AnimGetCellAlpha(AnimQueue *queue, int row, int col);
void ParticleEmit(ParticleSystem *ps, float x, float y, Color color, int count);
void ParticleUpdate(ParticleSystem *ps, float dt);
void ParticleDraw(ParticleSystem *ps);
void FloatTextAdd(FloatTextQueue *queue, const char *text, float centerX,
                  float startY, int fontSize, Color color);
void FloatTextUpdate(FloatTextQueue *queue, float dt);
void FloatTextDraw(FloatTextQueue *queue);

const LevelDef *LevelGetDefs(void);
void LevelInit(LevelState *state, int levelIndex, Board *board);
bool LevelCheckGoal(LevelState *state, int currentScore);
bool LevelCheckFailure(Board *board, PieceSlot slots[3]);

int ClampUnlockedLevel(int unlockedLevel);
void SaveLoad(int *highScore, int *unlockedLevel);
void SaveWrite(int highScore, int unlockedLevel);

void AssetsLoad(GameAssets *assets);
void AssetsUnload(GameAssets *assets);
void AssetsUpdateMusic(GameAssets *assets);
void SoundToggleSfx(GameAssets *assets);
void SoundToggleMusic(GameAssets *assets);
void SoundPlayPlace(GameAssets *assets);
void SoundPlayLineClear(GameAssets *assets, int linesCleared);
void SoundPlayCombo(GameAssets *assets, int comboCount);
void SoundPlayLose(GameAssets *assets);
void SoundPlayMenuClick(GameAssets *assets);

void RenderPlayHUD(GameState *state);
void RenderLevelSelect(GameState *state);
void RenderResult(GameState *state);
void RenderMenu(GameState *state);
void RenderSettings(GameState *state);
void RenderMenuSettings(GameState *state);
void RenderFrame(GameState *state);
void DrawSafeTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color c);
void DrawSafeQuad(Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4, Color c);
void DrawBlockBeveled(int x, int y, int size, Color baseColor);
void RenderBoard(GameState *state);
void RenderGhost(GameState *state);
void RenderPieceSlots(GameState *state);
void RenderDraggedPiece(GameState *state);
void RenderGearIcon(void);
void DrawGemIcon(int x, int y, int cellSize, int gemType);

void TryStartDrag(GameState *state, Vector2 mouse);
void AddClearFeedback(GameState *state,
                             bool clearedCells[GRID_SIZE][GRID_SIZE],
                             int savedColors[GRID_SIZE][GRID_SIZE], int points);
void GenerateNextPiecesIfNeeded(GameState *state);
void TryDropDraggedPiece(GameState *state);
void InputUpdate(GameState *state);

void GameInit(GameState *state);
void GameReset(GameState *state);
void GameUpdate(GameState *state);
void ExecuteSetting(GameState *state, int index);
void GameUpdateSettings(GameState *state);
void GameUpdateMenuSettings(GameState *state);

int main(void)
{
    MakeDirectory("data");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Blast");
    SetExitKey(0);
    SetTargetFPS(60);

    AssetsLoad(&assets);

    GameState state;
    GameInit(&state);

    while (!WindowShouldClose() && !state.shouldQuit) {
        AssetsUpdateMusic(&assets);

        if (state.currentScreen == SCREEN_PLAY) {
            InputUpdate(&state);
        }

        GameUpdate(&state);
        RenderFrame(&state);
    }

    AssetsUnload(&assets);
    CloseWindow();
    return 0;
}

/* ------------------------------------------------------------------------- */
/* Implementation                                                            */
/* ------------------------------------------------------------------------- */

/* ---- Piece implementation ---- */




const PieceDef *GetPieceDefinitions(void)
{
    return PIECE_DEFS;
}

void PieceGenerate(Piece *p, float diamondChance, float emeraldChance)
{
    if (!p) return;
    memset(p, 0, sizeof(Piece));

    int defIndex = rand() % PIECE_DEF_COUNT;
    const PieceDef *def = &PIECE_DEFS[defIndex];

    // Copy shape
    for (int r = 0; r < MAX_PIECE_SIZE; r++)
        for (int c = 0; c < MAX_PIECE_SIZE; c++)
            p->shape[r][c] = def->shape[r][c];

    p->width = def->width;
    p->height = def->height;
    p->colorIndex = (rand() % 7) + 1; // 1-7

    // Initialize gems
    memset(p->gemCells, GEM_NONE, sizeof(p->gemCells));

    // Randomly assign gems to filled cells
    if (diamondChance > 0.0f || emeraldChance > 0.0f) {
        for (int r = 0; r < p->height; r++) {
            for (int c = 0; c < p->width; c++) {
                if (p->shape[r][c] == 0) continue;

                float roll = (float)rand() / (float)RAND_MAX;
                if (emeraldChance > 0.0f && roll < emeraldChance) {
                    p->gemCells[r][c] = GEM_EMERALD;
                } else if (diamondChance > 0.0f && roll < diamondChance + emeraldChance) {
                    p->gemCells[r][c] = GEM_DIAMOND;
                }
                // else: GEM_NONE
            }
        }
    }

}

void SlotClear(PieceSlot *slot)
{
    if (!slot) return;
    memset(&slot->piece, 0, sizeof(Piece));
    slot->occupied = false;
}

bool SlotIsOccupied(const PieceSlot *slot)
{
    return slot && slot->occupied;
}

void GenerateRandomPieces(PieceSlot slots[3], float panelY, float screenWidth,
                           float diamondChance, float emeraldChance)
{
    // Divide the bottom panel into 3 equal sections
    float sectionWidth = screenWidth / 3.0f;

    for (int i = 0; i < 3; i++) {
        PieceGenerate(&slots[i].piece, diamondChance, emeraldChance);
        slots[i].occupied = true;

        // Center each piece in its section
        float piecePixelW = slots[i].piece.width * (float)PANEL_PIECE_SCALE;
        slots[i].posX = sectionWidth * i + (sectionWidth - piecePixelW) / 2.0f;
        slots[i].posY = panelY + 20.0f;
    }
}

bool AllSlotsEmpty(PieceSlot slots[3])
{
    for (int i = 0; i < 3; i++) {
        if (SlotIsOccupied(&slots[i])) return false;
    }
    return true;
}

/* ---- Board implementation ---- */

void BoardInit(Board *board)
{
    memset(board->cells, CELL_EMPTY, sizeof(board->cells));
    memset(board->gems, GEM_NONE, sizeof(board->gems));
}

bool BoardCanPlace(Board *board, Piece *piece, int row, int col)
{
    if (!piece) return false;

    for (int r = 0; r < piece->height; r++) {
        for (int c = 0; c < piece->width; c++) {
            if (piece->shape[r][c] == 0) continue;

            int gr = row + r;
            int gc = col + c;

            // Out of bounds check
            if (gr < 0 || gr >= GRID_SIZE || gc < 0 || gc >= GRID_SIZE)
                return false;

            // Collision check (empty, ice, or stone all block placement)
            if (board->cells[gr][gc] != CELL_EMPTY)
                return false;
        }
    }
    return true;
}

void BoardPlace(Board *board, Piece *piece, int row, int col)
{
    for (int r = 0; r < piece->height; r++) {
        for (int c = 0; c < piece->width; c++) {
            if (piece->shape[r][c] == 0) continue;
            board->cells[row + r][col + c] = piece->colorIndex;
            board->gems[row + r][col + c] = piece->gemCells[r][c];
        }
    }
}

int BoardClearLines(Board *board, bool clearedCells[GRID_SIZE][GRID_SIZE])
{
    int cleared = 0;
    bool rowFull[GRID_SIZE] = {false};
    bool colFull[GRID_SIZE] = {false};

    // Initialize clearedCells to false
    memset(clearedCells, false, sizeof(bool) * GRID_SIZE * GRID_SIZE);

    // Check which rows are full
    for (int r = 0; r < GRID_SIZE; r++) {
        rowFull[r] = true;
        for (int c = 0; c < GRID_SIZE; c++) {
            if (board->cells[r][c] == CELL_EMPTY) {
                rowFull[r] = false;
                break;
            }
        }
        if (rowFull[r]) cleared++;
    }

    // Check which columns are full
    for (int c = 0; c < GRID_SIZE; c++) {
        colFull[c] = true;
        for (int r = 0; r < GRID_SIZE; r++) {
            if (board->cells[r][c] == CELL_EMPTY) {
                colFull[c] = false;
                break;
            }
        }
        if (colFull[c]) cleared++;
    }

    // Mark and clear full rows
    for (int r = 0; r < GRID_SIZE; r++) {
        if (!rowFull[r]) continue;
        for (int c = 0; c < GRID_SIZE; c++) {
            clearedCells[r][c] = true;
            board->cells[r][c] = CELL_EMPTY;
            board->gems[r][c] = GEM_NONE;
        }
    }

    // Mark and clear full columns
    for (int c = 0; c < GRID_SIZE; c++) {
        if (!colFull[c]) continue;
        for (int r = 0; r < GRID_SIZE; r++) {
            clearedCells[r][c] = true;
            board->cells[r][c] = CELL_EMPTY;
            board->gems[r][c] = GEM_NONE;
        }
    }

    return cleared;
}


bool BoardHasValidMove(Board *board, PieceSlot slots[3])
{
    for (int s = 0; s < 3; s++) {
        if (!SlotIsOccupied(&slots[s])) continue;

        for (int r = 0; r <= GRID_SIZE - slots[s].piece.height; r++) {
            for (int c = 0; c <= GRID_SIZE - slots[s].piece.width; c++) {
                if (BoardCanPlace(board, &slots[s].piece, r, c))
                    return true;
            }
        }
    }
    return false;
}

void BoardPrefillGems(Board *board, int count, bool allowDiamonds,
                      bool allowEmeralds)
{
    // Use the piece definitions to get random shapes
    const PieceDef *defs = GetPieceDefinitions();

    for (int i = 0; i < count; i++) {
        // Pick a random piece definition
        int defIndex = rand() % PIECE_DEF_COUNT;
        const PieceDef *def = &defs[defIndex];

        // Try to find a valid position (max 50 attempts)
        for (int attempt = 0; attempt < 50; attempt++) {
            int row = rand() % (GRID_SIZE - def->height + 1);
            int col = rand() % (GRID_SIZE - def->width + 1);

            // Use BoardCanPlace logic via a temporary Piece
            Piece tempPiece;
            memset(&tempPiece, 0, sizeof(tempPiece));
            memcpy(tempPiece.shape, def->shape, sizeof(def->shape));
            tempPiece.width = def->width;
            tempPiece.height = def->height;
            tempPiece.colorIndex = 1;

            if (!BoardCanPlace(board, &tempPiece, row, col)) continue;

            // Place the piece with a random color
            int colorIdx = (rand() % 7) + 1;
            for (int r = 0; r < def->height; r++) {
                for (int c = 0; c < def->width; c++) {
                    if (def->shape[r][c] == 0) continue;
                    board->cells[row + r][col + c] = colorIdx;

                    // Assign only target-relevant gems to some cells.
                    if ((allowDiamonds || allowEmeralds) && (rand() % 100) < 30) {
                        if (allowDiamonds && allowEmeralds) {
                            board->gems[row + r][col + c] =
                                (rand() % 2) == 0 ? GEM_DIAMOND : GEM_EMERALD;
                        } else if (allowDiamonds) {
                            board->gems[row + r][col + c] = GEM_DIAMOND;
                        } else {
                            board->gems[row + r][col + c] = GEM_EMERALD;
                        }
                    }
                }
            }
            break; // Successfully placed
        }
    }
}

/* ---- Effects implementation ---- */

void AnimAddCleared(AnimQueue *queue, bool clearedCells[GRID_SIZE][GRID_SIZE])
{
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (!clearedCells[r][c]) continue;
            if (queue->count >= MAX_ANIMS * GRID_SIZE) return;

            queue->items[queue->count].active = true;
            queue->items[queue->count].timer = CLEAR_ANIM_DURATION;
            queue->items[queue->count].row = r;
            queue->items[queue->count].col = c;
            queue->count++;
        }
    }
}

bool AnimUpdate(AnimQueue *queue, float dt)
{
    bool anyActive = false;

    for (int i = 0; i < queue->count; i++) {
        if (!queue->items[i].active) continue;

        queue->items[i].timer -= dt;
        if (queue->items[i].timer <= 0.0f) {
            queue->items[i].active = false;
        } else {
            anyActive = true;
        }
    }

    // If no active animations, reset the queue
    if (!anyActive) {
        queue->count = 0;
    }

    return anyActive;
}

bool AnimIsActive(AnimQueue *queue)
{
    for (int i = 0; i < queue->count; i++) {
        if (queue->items[i].active) return true;
    }
    return false;
}

float AnimGetCellAlpha(AnimQueue *queue, int row, int col)
{
    for (int i = 0; i < queue->count; i++) {
        if (queue->items[i].active &&
            queue->items[i].row == row &&
            queue->items[i].col == col) {
            return queue->items[i].timer / CLEAR_ANIM_DURATION; // 1.0 -> 0.0 fade out
        }
    }
    return -1.0f; // not being animated
}

void ParticleEmit(ParticleSystem *ps, float x, float y,
                  Color color, int count)
{
    for (int i = 0; i < count; i++) {
        if (ps->count >= MAX_PARTICLES) return;

        ps->items[ps->count].active = true;
        ps->items[ps->count].pos = (Vector2){x, y};

        // Random direction and speed
        float angle = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
        float speed = 80.0f + ((float)rand() / (float)RAND_MAX) * 160.0f;
        ps->items[ps->count].vel = (Vector2){cosf(angle) * speed, sinf(angle) * speed};

        ps->items[ps->count].life = PARTICLE_LIFE + ((float)rand() / (float)RAND_MAX) * 0.4f;
        ps->items[ps->count].maxLife = ps->items[ps->count].life;
        ps->items[ps->count].size = 3.0f + ((float)rand() / (float)RAND_MAX) * 4.0f;
        ps->items[ps->count].gravity = 200.0f + ((float)rand() / (float)RAND_MAX) * 100.0f;

        // Slight color variation
        ps->items[ps->count].color = color;
        int variation = (rand() % 40) - 20;
        ps->items[ps->count].color.r = (unsigned char)fminf(255, fmaxf(0, color.r + variation));
        ps->items[ps->count].color.g = (unsigned char)fminf(255, fmaxf(0, color.g + variation));
        ps->count++;
    }
}

void ParticleUpdate(ParticleSystem *ps, float dt)
{
    bool anyActive = false;

    for (int i = 0; i < ps->count; i++) {
        if (!ps->items[i].active) continue;

        ps->items[i].life -= dt;
        if (ps->items[i].life <= 0.0f) {
            ps->items[i].active = false;
            continue;
        }

        anyActive = true;

        // Apply gravity
        ps->items[i].vel.y += ps->items[i].gravity * dt;

        // Move
        ps->items[i].pos.x += ps->items[i].vel.x * dt;
        ps->items[i].pos.y += ps->items[i].vel.y * dt;
    }

    // Reset count if all inactive
    if (!anyActive) ps->count = 0;
}

void ParticleDraw(ParticleSystem *ps)
{
    for (int i = 0; i < ps->count; i++) {
        if (!ps->items[i].active) continue;

        float t = ps->items[i].life / ps->items[i].maxLife;  // 1.0 -> 0.0

        // Size shrinks over time
        float currentSize = ps->items[i].size * t;
        if (currentSize < 0.5f) continue;

        // Alpha fades
        Color clr = ps->items[i].color;
        clr.a = (unsigned char)(t * 255.0f);

        DrawRectangle(
            (int)(ps->items[i].pos.x - currentSize / 2),
            (int)(ps->items[i].pos.y - currentSize / 2),
            (int)currentSize + 1,
            (int)currentSize + 1,
            clr
        );
    }
}

void FloatTextAdd(FloatTextQueue *queue, const char *text,
                  float centerX, float startY,
                  int fontSize, Color color)
{
    // Find an inactive slot or use the oldest one
    int idx = -1;
    for (int i = 0; i < queue->count; i++) {
        if (!queue->items[i].active) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        if (queue->count < MAX_FLOAT_TEXTS) {
            idx = queue->count++;
        } else {
            // Overwrite the oldest (first) item
            idx = 0;
        }
    }

    queue->items[idx].active = true;
    strncpy(queue->items[idx].text, text, FLOAT_TEXT_LEN - 1);
    queue->items[idx].text[FLOAT_TEXT_LEN - 1] = '\0';
    queue->items[idx].x = centerX;
    queue->items[idx].y = startY;
    queue->items[idx].startY = startY;
    queue->items[idx].timer = FLOAT_TEXT_DURATION;
    queue->items[idx].duration = FLOAT_TEXT_DURATION;
    queue->items[idx].speed = FLOAT_TEXT_SPEED;
    queue->items[idx].fontSize = fontSize;
    queue->items[idx].color = color;
}

void FloatTextUpdate(FloatTextQueue *queue, float dt)
{
    for (int i = 0; i < queue->count; i++) {
        if (!queue->items[i].active) continue;

        queue->items[i].timer -= dt;
        queue->items[i].y -= queue->items[i].speed * dt;  // move upward

        if (queue->items[i].timer <= 0.0f) {
            queue->items[i].active = false;
        }
    }

    // Compact: if all inactive, reset count
    bool anyActive = false;
    for (int i = 0; i < queue->count; i++) {
        if (queue->items[i].active) {
            anyActive = true;
            break;
        }
    }
    if (!anyActive) queue->count = 0;
}

void FloatTextDraw(FloatTextQueue *queue)
{
    for (int i = 0; i < queue->count; i++) {
        if (!queue->items[i].active) continue;

        float t = queue->items[i].timer / queue->items[i].duration;  // 1.0 -> 0.0
        unsigned char alpha = (unsigned char)(t * 255.0f);

        // Scale effect: start slightly larger, settle to normal
        float scale = 1.0f + (1.0f - t) * 0.15f;
        int fontSize = (int)(queue->items[i].fontSize * scale);

        Color clr = queue->items[i].color;
        clr.a = alpha;

        int w = (int)MeasureTextEx(assets.font, queue->items[i].text, (float)fontSize, 1.0f).x;
        DrawTextEx(assets.font, queue->items[i].text, (Vector2){queue->items[i].x - w / 2.0f, queue->items[i].y},
                   (float)fontSize, 1.0f, clr);
    }
}

/* ---- Level implementation ---- */



const LevelDef *LevelGetDefs(void)
{
    return defaultLevelDefs;
}

// ─── Level Init ───────────────────────────────────────────────────────────────
void LevelInit(LevelState *state, int levelIndex, Board *board)
{
    memset(state, 0, sizeof(LevelState));
    state->currentLevel = levelIndex;

    BoardInit(board);

    const LevelDef *level = &defaultLevelDefs[levelIndex];

    // Pre-fill board with random pieces (adventure levels only)
    if (level->prefillCount > 0) {
        BoardPrefillGems(board,
                         level->prefillCount,
                         level->targetDiamonds > 0,
                         level->targetEmeralds > 0);
    }
}

// ─── Goal Check ───────────────────────────────────────────────────────────────
bool LevelCheckGoal(LevelState *state, int currentScore)
{
    const LevelDef *level = &defaultLevelDefs[state->currentLevel];

    // Classic mode never wins
    if (level->level == 0) return false;

    // Check each target — if set (> 0) and not met, return false
    if (level->targetScore > 0 && currentScore < level->targetScore)
        return false;
    if (level->targetDiamonds > 0 && state->collectedDiamonds < level->targetDiamonds)
        return false;
    if (level->targetEmeralds > 0 && state->collectedEmeralds < level->targetEmeralds)
        return false;

    return true;
}

// ─── Failure Check ────────────────────────────────────────────────────────────
bool LevelCheckFailure(Board *board, PieceSlot slots[3])
{
    return !BoardHasValidMove(board, slots);
}

/* ---- Save implementation ---- */

#define SAVE_FILE "data/save.txt"

int ClampUnlockedLevel(int unlockedLevel)
{
    if (unlockedLevel < 1) return 1;
    if (unlockedLevel > TOTAL_LEVELS + 1) return TOTAL_LEVELS + 1;
    return unlockedLevel;
}

void SaveLoad(int *highScore, int *unlockedLevel)
{
    char line[128];
    FILE *f;

    if (highScore) *highScore = 0;
    if (unlockedLevel) *unlockedLevel = 1;

    f = fopen(SAVE_FILE, "r");
    if (!f) return;

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "highScore=", 10) == 0 && highScore) {
            int value = atoi(line + 10);
            if (value >= 0) *highScore = value;
        } else if (strncmp(line, "unlockedLevel=", 14) == 0 && unlockedLevel) {
            *unlockedLevel = ClampUnlockedLevel(atoi(line + 14));
        }
    }

    if (unlockedLevel) *unlockedLevel = ClampUnlockedLevel(*unlockedLevel);
    fclose(f);
}

void SaveWrite(int highScore, int unlockedLevel)
{
    FILE *f = fopen(SAVE_FILE, "w");
    if (!f) return;

    if (highScore < 0) highScore = 0;

    fprintf(f, "# Block Blast save file\n");
    fprintf(f, "highScore=%d\n", highScore);
    fprintf(f, "unlockedLevel=%d\n", ClampUnlockedLevel(unlockedLevel));
    fclose(f);
}

/* ---- Asset implementation ---- */

void AssetsLoad(GameAssets *assets)
{
    InitAudioDevice();

    assets->font = LoadFontEx("assets/RussoOne-Regular.ttf", 200, 0, 0);

    assets->crown = LoadTexture("assets/crown.png");
    assets->diamond = LoadTexture("assets/diamond.png");
    assets->emerald = LoadTexture("assets/emerald.png");
    assets->home = LoadTexture("assets/home.png");
    assets->lock = LoadTexture("assets/lock.png");
    assets->logout = LoadTexture("assets/logout.png");
    assets->musicalNote = LoadTexture("assets/musical-note.png");
    assets->waveSound = LoadTexture("assets/wave-sound.png");
    assets->replay = LoadTexture("assets/replay.png");
    assets->completed = LoadTexture("assets/completed.png");
    assets->logo = LoadTexture("assets/logo.png");

    assets->sfxPlace = LoadSound("assets/block-placing.ogg");
    assets->sfxLineClear1 = LoadSound("assets/line-clear-1.ogg");
    assets->sfxLineClear2 = LoadSound("assets/line-clear-2.ogg");
    assets->sfxCombo1 = LoadSound("assets/combo-1.ogg");
    assets->sfxCombo2 = LoadSound("assets/combo-2.ogg");
    assets->sfxLose = LoadSound("assets/lose.ogg");
    assets->sfxMenuClick = LoadSound("assets/menu-click.ogg");
    assets->bgMusic = LoadMusicStream("assets/background-music.wav");
    assets->bgMusic.looping = true;

    assets->sfxEnabled = true;
    assets->musicEnabled = true;

    SetSoundVolume(assets->sfxPlace, 0.7f);
    SetSoundVolume(assets->sfxLineClear1, 0.8f);
    SetSoundVolume(assets->sfxLineClear2, 0.8f);
    SetSoundVolume(assets->sfxCombo1, 0.9f);
    SetSoundVolume(assets->sfxCombo2, 0.9f);
    SetSoundVolume(assets->sfxLose, 0.8f);
    SetSoundVolume(assets->sfxMenuClick, 0.6f);
    SetMusicVolume(assets->bgMusic, 0.4f);

    PlayMusicStream(assets->bgMusic);
}

void AssetsUnload(GameAssets *assets)
{
    UnloadSound(assets->sfxPlace);
    UnloadSound(assets->sfxLineClear1);
    UnloadSound(assets->sfxLineClear2);
    UnloadSound(assets->sfxCombo1);
    UnloadSound(assets->sfxCombo2);
    UnloadSound(assets->sfxLose);
    UnloadSound(assets->sfxMenuClick);
    UnloadMusicStream(assets->bgMusic);

    UnloadTexture(assets->crown);
    UnloadTexture(assets->diamond);
    UnloadTexture(assets->emerald);
    UnloadTexture(assets->home);
    UnloadTexture(assets->lock);
    UnloadTexture(assets->logout);
    UnloadTexture(assets->musicalNote);
    UnloadTexture(assets->waveSound);
    UnloadTexture(assets->replay);
    UnloadTexture(assets->completed);
    UnloadTexture(assets->logo);

    UnloadFont(assets->font);
    CloseAudioDevice();
}

void AssetsUpdateMusic(GameAssets *assets)
{
    if (assets->musicEnabled) {
        UpdateMusicStream(assets->bgMusic);
    }
}

void SoundToggleSfx(GameAssets *assets)
{
    assets->sfxEnabled = !assets->sfxEnabled;
}

void SoundToggleMusic(GameAssets *assets)
{
    assets->musicEnabled = !assets->musicEnabled;
    if (assets->musicEnabled) {
        ResumeMusicStream(assets->bgMusic);
    } else {
        PauseMusicStream(assets->bgMusic);
    }
}

void SoundPlayPlace(GameAssets *assets)
{
    if (assets->sfxEnabled) PlaySound(assets->sfxPlace);
}

void SoundPlayLineClear(GameAssets *assets, int linesCleared)
{
    if (!assets->sfxEnabled) return;
    if (linesCleared >= 2)
        PlaySound(assets->sfxLineClear2);
    else
        PlaySound(assets->sfxLineClear1);
}

void SoundPlayCombo(GameAssets *assets, int comboCount)
{
    if (!assets->sfxEnabled) return;
    if (comboCount >= 4)
        PlaySound(assets->sfxCombo2);
    else
        PlaySound(assets->sfxCombo1);
}

void SoundPlayLose(GameAssets *assets)
{
    if (assets->sfxEnabled) PlaySound(assets->sfxLose);
}

void SoundPlayMenuClick(GameAssets *assets)
{
    if (assets->sfxEnabled) PlaySound(assets->sfxMenuClick);
}

/* ---- UI rendering implementation ---- */


// ─── Helper: draw texture full ───────────────────────────────────────────────


// ─── Helper: draw text centered X ────────────────────────────────────────────


// ─── Helper: draw styled button ──────────────────────────────────────────────


// ─── Helper: draw gem counter ────────────────────────────────────────────────


// ============================================================================
//  Play HUD — adapts based on level targets
// ============================================================================
void RenderPlayHUD(GameState *state)
{
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
    char buf[128];
    int textW;

    if (state->selectedLevel == 0) {
        DrawTexturePro(assets.crown,
            (Rectangle){0, 0, (float)assets.crown.width, (float)assets.crown.height},
            (Rectangle){15, 12, 28, 28},
            (Vector2){0, 0}, 0.0f, WHITE);

        sprintf(buf, "%d", state->highScore);
        DrawTextEx(assets.font, buf, (Vector2){51, 16}, 20.0f, 1.0f, (Color){255, 220, 50, 255});

        sprintf(buf, "%d", state->score);
        textW = (int)MeasureTextEx(assets.font, buf, 40.0f, 1.0f).x;
        DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, 40}, 40.0f, 1.0f, COLOR_TEXT_PRIMARY);

        if (state->combo > 1) {
            sprintf(buf, "COMBO x%d", state->combo);
            textW = (int)MeasureTextEx(assets.font, buf, 18.0f, 1.0f).x;
            DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, 80}, 18.0f, 1.0f, (Color){255, 220, 50, 255});
        }
        return;
    }

    bool hasScore = def->targetScore > 0;
    bool hasDiamond = def->targetDiamonds > 0;
    bool hasEmerald = def->targetEmeralds > 0;

    if (hasScore) {
        sprintf(buf, "%d / %d", state->score, def->targetScore);
        textW = (int)MeasureTextEx(assets.font, buf, 28.0f, 1.0f).x;
        DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, 49}, 28.0f, 1.0f, COLOR_TEXT_PRIMARY);
    }

    if (hasDiamond) {
        int remaining = def->targetDiamonds - state->level.collectedDiamonds;
        if (remaining < 0) remaining = 0;
        sprintf(buf, "%d", remaining);
        DrawTexturePro(assets.diamond,
            (Rectangle){0, 0, (float)assets.diamond.width, (float)assets.diamond.height},
            (Rectangle){99, 41, 32, 32},
            (Vector2){0, 0}, 0.0f, WHITE);
        textW = (int)MeasureTextEx(assets.font, buf, 22.0f, 1.0f).x;
        DrawTextEx(assets.font, buf, (Vector2){115 - textW / 2.0f, 79}, 22.0f, 1.0f, COLOR_TEXT_PRIMARY);
    }

    if (hasEmerald) {
        int remaining = def->targetEmeralds - state->level.collectedEmeralds;
        if (remaining < 0) remaining = 0;
        sprintf(buf, "%d", remaining);
        DrawTexturePro(assets.emerald,
            (Rectangle){0, 0, (float)assets.emerald.width, (float)assets.emerald.height},
            (Rectangle){349, 41, 32, 32},
            (Vector2){0, 0}, 0.0f, WHITE);
        textW = (int)MeasureTextEx(assets.font, buf, 22.0f, 1.0f).x;
        DrawTextEx(assets.font, buf, (Vector2){365 - textW / 2.0f, 79}, 22.0f, 1.0f, COLOR_TEXT_PRIMARY);
    }
}


// ============================================================================
//  Level Select Screen
// ============================================================================
void RenderLevelSelect(GameState *state)
{
    DrawTexturePro(assets.logout,
        (Rectangle){0, 0, (float)assets.logout.width, (float)assets.logout.height},
        (Rectangle){15, 15, 32, 32},
        (Vector2){0, 0}, 0.0f, WHITE);

    int textW;
    DrawTextEx(assets.font, "ADVENTURE", (Vector2){130, 30}, 36.0f, 1.0f, COLOR_TEXT_PRIMARY);

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        int row = i / LEVELS_PER_ROW;
        int col = i % LEVELS_PER_ROW;
        int x = 100 + col * 95;
        int y = 150 + row * 125;
        Rectangle button = {x, y, AMAP_BTN_SIZE, AMAP_BTN_SIZE};

        bool unlocked = LevelIsUnlocked(state->unlockedLevel, i + 1);
        bool completed = LevelIsCompleted(state->unlockedLevel, i + 1);
        bool hover = unlocked && CheckCollisionPointRec(mouse, button);

        Color bg = unlocked ? COLOR_AMAP_UNLOCKED_BG : COLOR_AMAP_LOCKED_BG;
        Color border = unlocked ? COLOR_AMAP_UNLOCKED_BORDER : COLOR_AMAP_LOCKED_BORDER;
        Color numberColor = unlocked ? COLOR_TEXT_PRIMARY : COLOR_AMAP_LOCKED_NUMBER;
        if (hover) bg = COLOR_AMAP_UNLOCKED_HOVER;

        DrawRectangleRounded(button, 0.2f, 6, bg);
        DrawRectangleRoundedLines(button, 0.2f, 6, border);

        char levelText[8];
        sprintf(levelText, "%d", i + 1);
        textW = (int)MeasureTextEx(assets.font, levelText, 28.0f, 1.0f).x;
        DrawTextEx(assets.font, levelText, (Vector2){x + (AMAP_BTN_SIZE - textW) / 2.0f, y + 20}, 28.0f, 1.0f, numberColor);

        if (completed) {
            DrawTexturePro(assets.completed,
                (Rectangle){0, 0, (float)assets.completed.width, (float)assets.completed.height},
                (Rectangle){x + 58, y - 6, 28, 28},
                (Vector2){0, 0}, 0.0f, WHITE);
        } else if (!unlocked) {
            DrawTexturePro(assets.lock,
                (Rectangle){0, 0, (float)assets.lock.width, (float)assets.lock.height},
                (Rectangle){x + 28, y + 48, 24, 24},
                (Vector2){0, 0}, 0.0f, WHITE);
        }
    }

    if (state->unlockedLevel > TOTAL_LEVELS) {
        DrawTextEx(assets.font, "Completed!", (Vector2){145, 690}, 28.0f, 1.0f, COLOR_AMAP_COMPLETED_TEXT);
    }

    DrawTextEx(assets.font, "ESC: Main Menu", (Vector2){178, SCREEN_HEIGHT - 30}, 16.0f, 1.0f, COLOR_TEXT_MUTED);
}


// ============================================================================
//  Result Screen (adaptive win/lose)
// ============================================================================
void RenderResult(GameState *state)
{
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
    char buf[128];
    Vector2 mouse = GetMousePosition();

    RenderPlayHUD(state);
    RenderBoard(state);
    RenderPieceSlots(state);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    int cardX = 50;
    int cardY = 180;
    int cardW = 380;
    int cardH = 400;
    DrawRectangleRounded((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, COLOR_SETTINGS_CARD_BG);
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, COLOR_SETTINGS_CARD_BORDER);

    const char *title = state->level.levelFailed ? (state->selectedLevel == 0 ? "GAME OVER" : "LEVEL FAILED!") : "LEVEL COMPLETE!";
    Color titleColor = state->level.levelFailed ? (Color){255, 80, 80, 255} : (Color){50, 255, 100, 255};
    int textW = (int)MeasureTextEx(assets.font, title, 32.0f, 1.0f).x;
    DrawTextEx(assets.font, title, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, cardY + 30}, 32.0f, 1.0f, titleColor);

    sprintf(buf, "Score: %d", state->score);
    textW = (int)MeasureTextEx(assets.font, buf, 28.0f, 1.0f).x;
    DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, cardY + 100}, 28.0f, 1.0f, WHITE);

    if (state->selectedLevel == 0) {
        sprintf(buf, "Best: %d", state->highScore);
        textW = (int)MeasureTextEx(assets.font, buf, 22.0f, 1.0f).x;
        DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, cardY + 140}, 22.0f, 1.0f, COLOR_TEXT_SECONDARY);
    } else {
        int y = cardY + 145;
        if (def->targetDiamonds > 0) {
            sprintf(buf, "Diamonds: %d / %d", state->level.collectedDiamonds, def->targetDiamonds);
            textW = (int)MeasureTextEx(assets.font, buf, 18.0f, 1.0f).x;
            DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, y}, 18.0f, 1.0f, (Color){100, 200, 255, 255});
            y += 30;
        }
        if (def->targetEmeralds > 0) {
            sprintf(buf, "Emeralds: %d / %d", state->level.collectedEmeralds, def->targetEmeralds);
            textW = (int)MeasureTextEx(assets.font, buf, 18.0f, 1.0f).x;
            DrawTextEx(assets.font, buf, (Vector2){(SCREEN_WIDTH - textW) / 2.0f, y}, 18.0f, 1.0f, (Color){50, 220, 100, 255});
        }
    }

    Rectangle topButton = {BTN_X, cardY + cardH - 140, BTN_W, BTN_H};
    Rectangle bottomButton = {BTN_X, cardY + cardH - 75, BTN_W, BTN_H};
    bool topHover = CheckCollisionPointRec(mouse, topButton);
    bool bottomHover = CheckCollisionPointRec(mouse, bottomButton);

    const char *topText = state->level.levelFailed ? (state->selectedLevel == 0 ? "Play Again" : "Try Again") : (state->level.currentLevel < TOTAL_LEVELS ? "Next Level" : "Completed!");
    Color topBg = topHover ? COLOR_BTN_ADV_BG_HOVER : COLOR_BTN_ADV_BG;
    Color topBorder = topHover ? COLOR_BTN_ADV_BORDER_HOVER : COLOR_BTN_ADV_BORDER;
    Color topTextColor = topHover ? COLOR_TEXT_PRIMARY : COLOR_BTN_ADV_TEXT;
    if (!state->level.levelFailed) {
        topBg = topHover ? (Color){50, 100, 60, 255} : (Color){35, 70, 45, 255};
        topBorder = topHover ? (Color){100, 220, 120, 255} : (Color){60, 140, 80, 255};
        topTextColor = topHover ? COLOR_TEXT_PRIMARY : (Color){150, 220, 170, 255};
    }

    DrawRectangleRounded(topButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, topBg);
    DrawRectangleRoundedLines(topButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, topBorder);
    textW = (int)MeasureTextEx(assets.font, topText, 18.0f, 1.0f).x;
    DrawTextEx(assets.font, topText, (Vector2){BTN_X + (BTN_W - textW) / 2.0f, topButton.y + 16}, 18.0f, 1.0f, topTextColor);

    DrawRectangleRounded(bottomButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, bottomHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255});
    DrawRectangleRoundedLines(bottomButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, bottomHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255});
    DrawTextEx(assets.font, "Main Menu", (Vector2){188, bottomButton.y + 16}, 18.0f, 1.0f, bottomHover ? COLOR_TEXT_PRIMARY : COLOR_SETTINGS_UNSELECTED_TEXT);

    DrawTextEx(assets.font, "ESC: Go Back", (Vector2){188, SCREEN_HEIGHT - 25}, 14.0f, 1.0f, COLOR_TEXT_MUTED);
}


// ─── Helper: draw styled button ──────────────────────────────────────────────


// ----- Menu screen -----
void RenderMenu(GameState *state) {
  // Draw Logo (Scaled to fit the screen)
  float targetW = SCREEN_WIDTH - 80.0f; // 40px margin on each side
  float scale = targetW / (float)assets.logo.width;
  float targetH = (float)assets.logo.height * scale;
  float logoX = (SCREEN_WIDTH - targetW) / 2.0f;
  float logoY = 50.0f; // Slightly higher to account for scaling
  DrawTexturePro(assets.logo,
                 (Rectangle){0, 0, (float)assets.logo.width, (float)assets.logo.height},
                 (Rectangle){logoX, logoY, targetW, targetH},
                 (Vector2){0, 0}, 0.0f, WHITE);

  Vector2 mouse = GetMousePosition();

  // --- Gear Icon (top-right) for menu settings ---
  RenderGearIcon();

  // --- Standard Mode Button ---
  Rectangle stdBtn = {BTN_X, MENU_STD_Y, BTN_W, BTN_H};
  bool stdHover = CheckCollisionPointRec(mouse, stdBtn);

  Color stdBg = stdHover ? COLOR_BTN_STD_BG_HOVER : COLOR_BTN_STD_BG;
  Color stdBorder =
      stdHover ? COLOR_BTN_STD_BORDER_HOVER : COLOR_BTN_STD_BORDER;

  DrawRectangleRounded(stdBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, stdBg);
  DrawRectangleRoundedLines(stdBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                            stdBorder);

  Color stdTextColor = stdHover ? COLOR_BTN_STD_TEXT_HOVER : COLOR_BTN_STD_TEXT;
  DrawTextEx(assets.font, "Classic Mode",
             (Vector2){151, (float)(MENU_STD_Y + (BTN_H - 22) / 2)},
             22.0f, 1.0f, stdTextColor);

  // --- Adventure Mode Button ---
  Rectangle advBtn = {BTN_X, MENU_ADV_Y, BTN_W, BTN_H};
  bool advHover = CheckCollisionPointRec(mouse, advBtn);

  Color advBg = advHover ? COLOR_BTN_ADV_BG_HOVER : COLOR_BTN_ADV_BG;
  Color advBorder =
      advHover ? COLOR_BTN_ADV_BORDER_HOVER : COLOR_BTN_ADV_BORDER;

  DrawRectangleRounded(advBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, advBg);
  DrawRectangleRoundedLines(advBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS,
                            advBorder);

  Color advTextColor = advHover ? COLOR_BTN_ADV_TEXT_HOVER : COLOR_BTN_ADV_TEXT;
  DrawTextEx(assets.font, "Adventure Mode",
             (Vector2){134, (float)(MENU_ADV_Y + (BTN_H - 22) / 2)},
             22.0f, 1.0f, advTextColor);

  if (state->unlockedLevel > TOTAL_LEVELS) {
      DrawTextEx(assets.font, "Completed",
                 (Vector2){198, (float)(MENU_ADV_Y + BTN_H + 5)},
                 14.0f, 1.0f, (Color){ 50,  255, 100, 255 });
  }

  // --- Quit Button ---
  Rectangle quitBtn = {BTN_X, MENU_QUIT_Y, BTN_W, BTN_H};
  bool quitHover = CheckCollisionPointRec(mouse, quitBtn);
  Color quitBg = quitHover ? COLOR_BTN_QUIT_BG_HOVER : COLOR_BTN_QUIT_BG;
  Color quitBorder = quitHover ? COLOR_BTN_QUIT_BORDER_HOVER : COLOR_BTN_QUIT_BORDER;
  Color quitTextColor = quitHover ? COLOR_BTN_QUIT_TEXT_HOVER : COLOR_BTN_QUIT_TEXT;
  DrawRectangleRounded(quitBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, quitBg);
  DrawRectangleRoundedLines(quitBtn, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, quitBorder);
  DrawTextEx(assets.font, "Quit",
             (Vector2){217, (float)(MENU_QUIT_Y + (BTN_H - 22) / 2)},
             22.0f, 1.0f, quitTextColor);

  // --- Game Simulation Decoration ---
  int s = 35; // Block size for mini grid
  int startX = (SCREEN_WIDTH - (4 * s)) / 2;
  int startY = 560;

  // 1. Draw Grid Background
  DrawRectangle(startX - 4, startY - 4, 4*s + 8, 4*s + 8, (Color){ 50,  50,  70, 255 });

  // 2. Draw empty cells
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      DrawRectangle(startX + c*s + 1, startY + r*s + 1, s - 2, s - 2, (Color){ 40,  40,  60, 255 });
    }
  }

  // 3. Placed Piece: Blue L-shape (left side)
  Color colBlue = PIECE_COLORS[4];
  DrawBlockBeveled(startX + 0*s, startY + 1*s, s, colBlue); // Row 1, Col 0
  DrawBlockBeveled(startX + 0*s, startY + 2*s, s, colBlue); // Row 2, Col 0
  DrawBlockBeveled(startX + 0*s, startY + 3*s, s, colBlue); // Row 3, Col 0
  DrawBlockBeveled(startX + 1*s, startY + 3*s, s, colBlue); // Row 3, Col 1

  // 4. Placed Piece: Yellow 2x2 Square (bottom right)
  Color colYellow = PIECE_COLORS[2];
  DrawBlockBeveled(startX + 2*s, startY + 2*s, s, colYellow); // Row 2, Col 2
  DrawBlockBeveled(startX + 3*s, startY + 2*s, s, colYellow); // Row 2, Col 3
  DrawBlockBeveled(startX + 2*s, startY + 3*s, s, colYellow); // Row 3, Col 2
  DrawBlockBeveled(startX + 3*s, startY + 3*s, s, colYellow); // Row 3, Col 3

  // 5. Hovering Piece: Red 3x1 horizontal
  Color colRed = PIECE_COLORS[1];
  Color ghostRed = colRed;
  ghostRed.a = 80;

  // Ghost indicator on the grid
  DrawBlockBeveled(startX + 1*s, startY + 0*s, s, ghostRed); // Row 0, Col 1
  DrawBlockBeveled(startX + 2*s, startY + 0*s, s, ghostRed); // Row 0, Col 2
  DrawBlockBeveled(startX + 3*s, startY + 0*s, s, ghostRed); // Row 0, Col 3

  // Actual hovering piece (offset upwards to simulate dragging)
  int hoverOffsetY = -18;
  int hoverOffsetX = 6;
  DrawBlockBeveled(startX + 1*s + hoverOffsetX, startY + 0*s + hoverOffsetY, s, colRed);
  DrawBlockBeveled(startX + 2*s + hoverOffsetX, startY + 0*s + hoverOffsetY, s, colRed);
  DrawBlockBeveled(startX + 3*s + hoverOffsetX, startY + 0*s + hoverOffsetY, s, colRed);
}

// ----- Settings screen (overlay with framed card) - for in-game settings -----
void RenderSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();
    int cardX = 80;
    int cardY = 165;
    int iconY = 250;
    int sfxIconX = 160;
    int musicIconX = 280;
    int replayY = 330;
    int homeY = 395;
    Rectangle replayButton = {BTN_X, replayY, BTN_W, BTN_H};
    Rectangle homeButton = {BTN_X, homeY, BTN_W, BTN_H};

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SETTINGS_OVERLAY_BG);
    DrawRectangleRounded((Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT}, 0.15f, 8, COLOR_SETTINGS_CARD_BG);
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT}, 0.15f, 8, COLOR_SETTINGS_CARD_BORDER);

    DrawTextEx(assets.font, "SETTINGS", (Vector2){158, cardY + 25}, 30.0f, 1.0f, WHITE);
    DrawLineEx((Vector2){120, cardY + 65}, (Vector2){360, cardY + 65}, 1.5f, COLOR_SETTINGS_SEPARATOR);

    DrawTexturePro(assets.waveSound, (Rectangle){0, 0, (float)assets.waveSound.width, (float)assets.waveSound.height}, (Rectangle){sfxIconX, iconY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
    if (!assets.sfxEnabled) DrawLineEx((Vector2){sfxIconX + 2, iconY + 38}, (Vector2){sfxIconX + 38, iconY + 2}, 5.0f, RED);
    DrawTextEx(assets.font, "SFX", (Vector2){166, iconY + 44}, 14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    DrawTexturePro(assets.musicalNote, (Rectangle){0, 0, (float)assets.musicalNote.width, (float)assets.musicalNote.height}, (Rectangle){musicIconX, iconY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
    if (!assets.musicEnabled) DrawLineEx((Vector2){musicIconX + 2, iconY + 38}, (Vector2){musicIconX + 38, iconY + 2}, 5.0f, RED);
    DrawTextEx(assets.font, "Music", (Vector2){279, iconY + 44}, 14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    bool replayHover = CheckCollisionPointRec(mouse, replayButton);
    DrawRectangleRounded(replayButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, replayHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255});
    DrawRectangleRoundedLines(replayButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, replayHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255});
    DrawTexturePro(assets.replay, (Rectangle){0, 0, (float)assets.replay.width, (float)assets.replay.height}, (Rectangle){BTN_X + 15, replayY + 15, 20, 20}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTextEx(assets.font, "Replay", (Vector2){206, replayY + 16}, 18.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    bool homeHover = CheckCollisionPointRec(mouse, homeButton);
    DrawRectangleRounded(homeButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, homeHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255});
    DrawRectangleRoundedLines(homeButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, homeHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255});
    DrawTexturePro(assets.home, (Rectangle){0, 0, (float)assets.home.width, (float)assets.home.height}, (Rectangle){BTN_X + 15, homeY + 15, 20, 20}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTextEx(assets.font, "Home", (Vector2){215, homeY + 16}, 18.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    DrawTextEx(assets.font, "ESC to go back", (Vector2){180, cardY + SETTINGS_CARD_HEIGHT - 30}, 14.0f, 1.0f, COLOR_SETTINGS_FOOTER_HINT);
}


// ----- Menu Settings screen (from main menu gear icon) -----
void RenderMenuSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();
    int cardX = 80;
    int cardY = 165;
    int iconY = 250;
    int sfxIconX = 160;
    int musicIconX = 280;
    int resetY = 330;
    int homeY = 395;
    Rectangle resetButton = {BTN_X, resetY, BTN_W, BTN_H};
    Rectangle homeButton = {BTN_X, homeY, BTN_W, BTN_H};

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SETTINGS_OVERLAY_BG);
    DrawRectangleRounded((Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT}, 0.15f, 8, COLOR_SETTINGS_CARD_BG);
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, SETTINGS_CARD_WIDTH, SETTINGS_CARD_HEIGHT}, 0.15f, 8, COLOR_SETTINGS_CARD_BORDER);

    DrawTextEx(assets.font, "SETTINGS", (Vector2){158, cardY + 25}, 30.0f, 1.0f, WHITE);
    DrawLineEx((Vector2){120, cardY + 65}, (Vector2){360, cardY + 65}, 1.5f, COLOR_SETTINGS_SEPARATOR);

    DrawTexturePro(assets.waveSound, (Rectangle){0, 0, (float)assets.waveSound.width, (float)assets.waveSound.height}, (Rectangle){sfxIconX, iconY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
    if (!assets.sfxEnabled) DrawLineEx((Vector2){sfxIconX + 2, iconY + 38}, (Vector2){sfxIconX + 38, iconY + 2}, 5.0f, RED);
    DrawTextEx(assets.font, "SFX", (Vector2){166, iconY + 44}, 14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    DrawTexturePro(assets.musicalNote, (Rectangle){0, 0, (float)assets.musicalNote.width, (float)assets.musicalNote.height}, (Rectangle){musicIconX, iconY, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
    if (!assets.musicEnabled) DrawLineEx((Vector2){musicIconX + 2, iconY + 38}, (Vector2){musicIconX + 38, iconY + 2}, 5.0f, RED);
    DrawTextEx(assets.font, "Music", (Vector2){279, iconY + 44}, 14.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    bool resetHover = CheckCollisionPointRec(mouse, resetButton);
    DrawRectangleRounded(resetButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, resetHover ? COLOR_BTN_QUIT_BG_HOVER : COLOR_BTN_QUIT_BG);
    DrawRectangleRoundedLines(resetButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, resetHover ? COLOR_BTN_QUIT_BORDER_HOVER : COLOR_BTN_QUIT_BORDER);
    DrawTextEx(assets.font, "Reset Save", (Vector2){181, resetY + 16}, 18.0f, 1.0f, resetHover ? COLOR_BTN_QUIT_TEXT_HOVER : COLOR_BTN_QUIT_TEXT);

    bool homeHover = CheckCollisionPointRec(mouse, homeButton);
    DrawRectangleRounded(homeButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, homeHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255});
    DrawRectangleRoundedLines(homeButton, BTN_CORNER_RADIUS, BTN_BORDER_SEGMENTS, homeHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255});
    DrawTexturePro(assets.home, (Rectangle){0, 0, (float)assets.home.width, (float)assets.home.height}, (Rectangle){BTN_X + 15, homeY + 15, 20, 20}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTextEx(assets.font, "Home", (Vector2){215, homeY + 16}, 18.0f, 1.0f, COLOR_SETTINGS_UNSELECTED_TEXT);

    DrawTextEx(assets.font, "ESC to go back", (Vector2){180, cardY + SETTINGS_CARD_HEIGHT - 30}, 14.0f, 1.0f, COLOR_SETTINGS_FOOTER_HINT);
}


/* ---- Gameplay rendering implementation ---- */

void RenderFrame(GameState *state)
{
    BeginDrawing();
    ClearBackground(BG_COLOR);

    switch (state->currentScreen) {
        case SCREEN_MENU:
            RenderMenu(state);
            break;
        case SCREEN_PLAY:
            RenderPlayHUD(state);
            RenderBoard(state);
            RenderGhost(state);
            RenderPieceSlots(state);
            RenderDraggedPiece(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            RenderGearIcon();
            break;
        case SCREEN_SETTINGS:
            // Draw the game behind the overlay
            RenderPlayHUD(state);
            RenderBoard(state);
            RenderPieceSlots(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            // Settings overlay on top
            RenderSettings(state);
            break;
        case SCREEN_LEVEL_SELECT:
            RenderLevelSelect(state);
            break;
        case SCREEN_RESULT:
            RenderResult(state);
            break;
        case SCREEN_MENU_SETTINGS:
            RenderMenuSettings(state);
            break;
    }

    EndDrawing();
}

// ----- Block Rendering -----
void DrawSafeTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color c) {
    DrawTriangle(v1, v2, v3, c);
    DrawTriangle(v1, v3, v2, c);
}

void DrawSafeQuad(Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4, Color c) {
    DrawSafeTriangle(v1, v2, v3, c);
    DrawSafeTriangle(v1, v3, v4, c);
}

void DrawBlockBeveled(int x, int y, int size, Color baseColor)
{
    Color lightColor = (Color){
        (unsigned char)(baseColor.r + (255 - baseColor.r) * 0.5f),
        (unsigned char)(baseColor.g + (255 - baseColor.g) * 0.5f),
        (unsigned char)(baseColor.b + (255 - baseColor.b) * 0.5f),
        baseColor.a
    };
    Color darkColor = (Color){
        (unsigned char)(baseColor.r * 0.5f),
        (unsigned char)(baseColor.g * 0.5f),
        (unsigned char)(baseColor.b * 0.5f),
        baseColor.a
    };
    Color veryDarkColor = (Color){
        (unsigned char)(baseColor.r * 0.3f),
        (unsigned char)(baseColor.g * 0.3f),
        (unsigned char)(baseColor.b * 0.3f),
        baseColor.a
    };

    int margin = 1;
    int bx = x + margin;
    int by = y + margin;
    int bsize = size - 2 * margin;
    int b = bsize / 10; // Bevel width halved
    if (b < 2) b = 2;

    Vector2 tl = { (float)bx, (float)by };
    Vector2 tr = { (float)(bx + bsize), (float)by };
    Vector2 bl = { (float)bx, (float)(by + bsize) };
    Vector2 br = { (float)(bx + bsize), (float)(by + bsize) };

    Vector2 itl = { (float)(bx + b), (float)(by + b) };
    Vector2 itr = { (float)(bx + bsize - b), (float)(by + b) };
    Vector2 ibl = { (float)(bx + b), (float)(by + bsize - b) };
    Vector2 ibr = { (float)(bx + bsize - b), (float)(by + bsize - b) };

    // Top bevel (light)
    DrawSafeQuad(tl, tr, itr, itl, lightColor);
    // Left bevel (light)
    DrawSafeQuad(tl, itl, ibl, bl, lightColor);
    // Right bevel (dark)
    DrawSafeQuad(tr, br, ibr, itr, darkColor);
    // Bottom bevel (very dark)
    DrawSafeQuad(bl, ibl, ibr, br, veryDarkColor);

    // Inner center
    DrawRectangle(bx + b, by + b, bsize - 2 * b, bsize - 2 * b, baseColor);

    // Inner highlight for jewel effect
    int ib = b / 2;
    if (ib > 0) {
        DrawRectangle(bx + b, by + b, (bsize - 2 * b), ib, lightColor);
        DrawRectangle(bx + b, by + b, ib, (bsize - 2 * b), lightColor);
    }
}

// ----- Board -----
void RenderBoard(GameState *state)
{
    // Draw board background
    DrawRectangle(GRID_DRAW_X - 4, GRID_DRAW_Y - 4,
                  GRID_SIZE * CELL_SIZE + 8, GRID_SIZE * CELL_SIZE + 8,
                  COLOR_GRID_BORDER);

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            int x = GRID_DRAW_X + c * CELL_SIZE;
            int y = GRID_DRAW_Y + r * CELL_SIZE;
            int val = state->board.cells[r][c];
            int gem = state->board.gems[r][c];

            // Check if cell is being animated
            float alpha = AnimGetCellAlpha(&state->anims, r, c);
            if (alpha >= 0.0f) {
                // Animated cell: fade out
                Color clr = PIECE_COLORS[val > 0 ? val : 1];
                clr.a = (unsigned char)(alpha * 255);
                DrawBlockBeveled(x, y, CELL_SIZE, clr);
            } else if (val != CELL_EMPTY) {
                // Normal filled cell
                Color clr = PIECE_COLORS[val];
                DrawBlockBeveled(x, y, CELL_SIZE, clr);

                // Draw gem icon if present
                if (gem != GEM_NONE) {
                    DrawGemIcon(x, y, CELL_SIZE, gem);
                }
            } else {
                // Empty cell
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, COLOR_GRID_EMPTY);
            }
        }
    }
}

// ----- Ghost preview -----
void RenderGhost(GameState *state)
{
    if (!state->isDragging) return;

    if (!SlotIsOccupied(&state->slots[state->dragSlotIndex])) return;

    float pieceScreenX = state->dragPos.x - state->dragOffset.x;
    float pieceScreenY = state->dragPos.y - state->dragOffset.y;

    int gridCol = (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE/2) / CELL_SIZE);
    int gridRow = (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE/2) / CELL_SIZE);

    if (!BoardCanPlace(&state->board, &state->slots[state->dragSlotIndex].piece, gridRow, gridCol)) return;

    Color ghost = PIECE_COLORS[state->slots[state->dragSlotIndex].piece.colorIndex];
    ghost.a = 80;

    for (int r = 0; r < state->slots[state->dragSlotIndex].piece.height; r++) {
        for (int c = 0; c < state->slots[state->dragSlotIndex].piece.width; c++) {
            if (state->slots[state->dragSlotIndex].piece.shape[r][c] == 0) continue;
            int x = GRID_DRAW_X + (gridCol + c) * CELL_SIZE;
            int y = GRID_DRAW_Y + (gridRow + r) * CELL_SIZE;
            DrawBlockBeveled(x, y, CELL_SIZE, ghost);
        }
    }
}

// ----- Piece slots (bottom panel) -----
void RenderPieceSlots(GameState *state)
{
    // Panel background
    DrawRectangle(0, PANEL_Y - 10, SCREEN_WIDTH, SCREEN_HEIGHT - PANEL_Y + 10, COLOR_PANEL_BG);
    DrawRectangle(0, PANEL_Y - 10, SCREEN_WIDTH, (int)PANEL_TOP_BORDER_WIDTH, COLOR_PANEL_TOP_BORDER);

    for (int i = 0; i < 3; i++) {
        // Skip the slot being dragged
        if (state->isDragging && state->dragSlotIndex == i) continue;

        if (!SlotIsOccupied(&state->slots[i])) continue;

        Color clr = PIECE_COLORS[state->slots[i].piece.colorIndex];

        for (int r = 0; r < state->slots[i].piece.height; r++) {
            for (int c = 0; c < state->slots[i].piece.width; c++) {
                if (state->slots[i].piece.shape[r][c] == 0) continue;
                int x = (int)(state->slots[i].posX + c * PANEL_PIECE_SCALE);
                int y = (int)(state->slots[i].posY + r * PANEL_PIECE_SCALE);
                DrawBlockBeveled(x, y, PANEL_PIECE_SCALE, clr);

                // Draw gem icon in panel if present
                if (state->slots[i].piece.gemCells[r][c] != GEM_NONE) {
                    DrawGemIcon(x, y, PANEL_PIECE_SCALE, state->slots[i].piece.gemCells[r][c]);
                }
            }
        }
    }
}

// ----- Dragged piece (follows mouse at grid scale) -----
void RenderDraggedPiece(GameState *state)
{
    if (!state->isDragging) return;

    if (!SlotIsOccupied(&state->slots[state->dragSlotIndex])) return;

    float baseX = state->dragPos.x - state->dragOffset.x;
    float baseY = state->dragPos.y - state->dragOffset.y;

    Color clr = PIECE_COLORS[state->slots[state->dragSlotIndex].piece.colorIndex];
    clr.a = 200;

    for (int r = 0; r < state->slots[state->dragSlotIndex].piece.height; r++) {
        for (int c = 0; c < state->slots[state->dragSlotIndex].piece.width; c++) {
            if (state->slots[state->dragSlotIndex].piece.shape[r][c] == 0) continue;
            int x = (int)(baseX + c * CELL_SIZE);
            int y = (int)(baseY + r * CELL_SIZE);
            DrawBlockBeveled(x, y, CELL_SIZE, clr);

            // Draw gem icon on dragged piece
            if (state->slots[state->dragSlotIndex].piece.gemCells[r][c] != GEM_NONE) {
                DrawGemIcon(x, y, CELL_SIZE, state->slots[state->dragSlotIndex].piece.gemCells[r][c]);
            }
        }
    }
}

// ----- Gear icon (settings button, top-right) -----
void RenderGearIcon(void)
{
    int cx = GEAR_X + GEAR_SIZE / 2;
    int cy = GEAR_Y + GEAR_SIZE / 2;
    int outerR = GEAR_SIZE / 2 - 2;
    int innerR = outerR - 7;
    int holeR  = 5;

    Color gearClr = (Color){180, 180, 200, 200};
    Vector2 mousePos = GetMousePosition();
    Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
    if (CheckCollisionPointRec(mousePos, gearRect)) {
        gearClr = (Color){255, 220, 50, 240}; // highlight on hover
    }

    // Outer circle
    DrawCircle(cx, cy, outerR, gearClr);

    // Gear teeth (8 small rectangles around the circle)
    for (int i = 0; i < 8; i++) {
        float angle = (float)i * (PI / 4.0f);
        float tx = cx + cosf(angle) * (outerR - 1);
        float ty = cy + sinf(angle) * (outerR - 1);
        DrawRectanglePro(
            (Rectangle){ tx, ty, 8, 5 },
            (Vector2){ 4, 2.5f },
            angle * (180.0f / PI),
            gearClr
        );
    }

    // Inner circle (darker, to create the ring look)
    DrawCircle(cx, cy, innerR, (Color){30, 30, 48, 255});

    // Center hole
    DrawCircle(cx, cy, holeR, gearClr);
    DrawCircle(cx, cy, holeR - 2, (Color){30, 30, 48, 255});
}

// ----- Draw a gem icon inside a cell block (using PNG textures) -----
void DrawGemIcon(int x, int y, int cellSize, int gemType)
{
    int gemSize = (int)(cellSize * GEM_SIZE_RATIO);
    int cx = x + cellSize / 2;
    int cy = y + cellSize / 2;

    if (gemType == GEM_DIAMOND) {
        DrawTexturePro(assets.diamond,
            (Rectangle){ 0, 0, (float)assets.diamond.width, (float)assets.diamond.height },
            (Rectangle){ (float)(cx - gemSize/2), (float)(cy - gemSize/2), (float)gemSize, (float)gemSize },
            (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else if (gemType == GEM_EMERALD) {
        DrawTexturePro(assets.emerald,
            (Rectangle){ 0, 0, (float)assets.emerald.width, (float)assets.emerald.height },
            (Rectangle){ (float)(cx - gemSize/2), (float)(cy - gemSize/2), (float)gemSize, (float)gemSize },
            (Vector2){ 0, 0 }, 0.0f, WHITE);
    }
}

/* ---- Input implementation ---- */

void TryStartDrag(GameState *state, Vector2 mouse)
{
  if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || state->isDragging)
    return;

  for (int i = 0; i < 3; i++) {
    if (!SlotIsOccupied(&state->slots[i]))
      continue;

    float pw = state->slots[i].piece.width * PANEL_PIECE_SCALE;
    float ph = state->slots[i].piece.height * PANEL_PIECE_SCALE;
    Rectangle rect = {state->slots[i].posX, state->slots[i].posY, pw, ph};

    if (CheckCollisionPointRec(mouse, rect)) {
      state->isDragging = true;
      state->dragSlotIndex = i;
      state->dragOffset.x = mouse.x - state->slots[i].posX;
      state->dragOffset.y = mouse.y - state->slots[i].posY;
      state->dragPos = mouse;
      break;
    }
  }
}

void AddClearFeedback(GameState *state,
                             bool clearedCells[GRID_SIZE][GRID_SIZE],
                             int savedColors[GRID_SIZE][GRID_SIZE],
                             int points)
{
  AnimAddCleared(&state->anims, clearedCells);

  for (int r = 0; r < GRID_SIZE; r++) {
    for (int c = 0; c < GRID_SIZE; c++) {
      if (!clearedCells[r][c])
        continue;
      float px = GRID_DRAW_X + c * CELL_SIZE + CELL_SIZE / 2.0f;
      float py = GRID_DRAW_Y + r * CELL_SIZE + CELL_SIZE / 2.0f;
      int colorIdx = savedColors[r][c];
      if (colorIdx < 1 || colorIdx > 7)
        colorIdx = 1;
      ParticleEmit(&state->particles, px, py, PIECE_COLORS[colorIdx], 5);
    }
  }

  char scoreBuf[64];
  if (state->combo > 1)
    sprintf(scoreBuf, "+%d (x%d Combo)", points, state->combo);
  else
    sprintf(scoreBuf, "+%d", points);
  FloatTextAdd(&state->floatTexts, scoreBuf, SCREEN_WIDTH / 2.0f,
               SCREEN_HEIGHT / 2.0f - 40.0f, 36,
               (Color){255, 255, 100, 255});

  if (state->combo >= 3) {
    char comboBanner[32];
    if (state->combo >= 5)
      sprintf(comboBanner, "AMAZING! x%d", state->combo);
    else if (state->combo >= 4)
      sprintf(comboBanner, "SUPER COMBO! x%d", state->combo);
    else
      sprintf(comboBanner, "BEST COMBO! x%d", state->combo);
    FloatTextAdd(&state->floatTexts, comboBanner, SCREEN_WIDTH / 2.0f,
                 SCREEN_HEIGHT / 2.0f - 100.0f, 40,
                 (Color){255, 220, 50, 255});
  }
}

void GenerateNextPiecesIfNeeded(GameState *state)
{
  if (!AllSlotsEmpty(state->slots))
    return;

  const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
  float diamondChance = (def->targetDiamonds > 0) ? DIAMOND_SPAWN_CHANCE : 0.0f;
  float emeraldChance = (def->targetEmeralds > 0) ? EMERALD_SPAWN_CHANCE : 0.0f;
  GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance,
                       emeraldChance);

  for (int i = 0; i < 3; i++) {
    if (!SlotIsOccupied(&state->slots[i]))
      continue;

    float slotCenterX =
        state->slots[i].posX + (state->slots[i].piece.width * PANEL_PIECE_SCALE) / 2.0f;
    float slotCenterY =
        state->slots[i].posY + (state->slots[i].piece.height * PANEL_PIECE_SCALE) / 2.0f;
    int colorIdx = state->slots[i].piece.colorIndex;
    if (colorIdx < 1 || colorIdx > 7)
      colorIdx = 1;
    ParticleEmit(&state->particles, slotCenterX, slotCenterY,
                 PIECE_COLORS[colorIdx], 8);
  }
}

void TryDropDraggedPiece(GameState *state)
{
  if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || !state->isDragging)
    return;

  state->isDragging = false;

  if (!SlotIsOccupied(&state->slots[state->dragSlotIndex]))
    return;

  float pieceScreenX = state->dragPos.x - state->dragOffset.x;
  float pieceScreenY = state->dragPos.y - state->dragOffset.y;

  int gridCol =
      (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE / 2) / CELL_SIZE);
  int gridRow =
      (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE / 2) / CELL_SIZE);

  if (!BoardCanPlace(&state->board, &state->slots[state->dragSlotIndex].piece, gridRow, gridCol))
    return;

  BoardPlace(&state->board, &state->slots[state->dragSlotIndex].piece, gridRow, gridCol);
  SoundPlayPlace(&assets);

  float placeCenterX =
      GRID_DRAW_X + gridCol * CELL_SIZE + (state->slots[state->dragSlotIndex].piece.width * CELL_SIZE) / 2.0f;
  float placeCenterY =
      GRID_DRAW_Y + gridRow * CELL_SIZE + (state->slots[state->dragSlotIndex].piece.height * CELL_SIZE) / 2.0f;
  int colorIdx = state->slots[state->dragSlotIndex].piece.colorIndex;
  if (colorIdx < 1 || colorIdx > 7)
    colorIdx = 1;
  ParticleEmit(&state->particles, placeCenterX, placeCenterY,
               PIECE_COLORS[colorIdx], 12);

  int savedColors[GRID_SIZE][GRID_SIZE];
  int savedGems[GRID_SIZE][GRID_SIZE];
  memcpy(savedColors, state->board.cells, sizeof(savedColors));
  memcpy(savedGems, state->board.gems, sizeof(savedGems));

  bool clearedCells[GRID_SIZE][GRID_SIZE];
  int linesCleared = BoardClearLines(&state->board, clearedCells);
  int diamondsCollected = 0;
  int emeraldsCollected = 0;

  for (int r = 0; r < GRID_SIZE; r++) {
    for (int c = 0; c < GRID_SIZE; c++) {
      if (clearedCells[r][c] && savedGems[r][c] == GEM_DIAMOND)
        diamondsCollected++;
      else if (clearedCells[r][c] && savedGems[r][c] == GEM_EMERALD)
        emeraldsCollected++;
    }
  }

  if (linesCleared > 0) {
    state->combo += linesCleared;
    int points = ScoreCalculate(linesCleared, state->combo);
    state->score += points;

    SoundPlayLineClear(&assets, linesCleared);
    if (state->combo >= 2)
      SoundPlayCombo(&assets, state->combo);

    AddClearFeedback(state, clearedCells, savedColors, points);

    if (state->selectedLevel > 0) {
      state->level.collectedDiamonds += diamondsCollected;
      state->level.collectedEmeralds += emeraldsCollected;
    }
  } else {
    state->combo = 0;
  }

  if (state->score > state->highScore)
    state->highScore = state->score;

  SlotClear(&state->slots[state->dragSlotIndex]);
  GenerateNextPiecesIfNeeded(state);
}

void InputUpdate(GameState *state) {
  if (AnimIsActive(&state->anims))
    return;
  if (state->level.levelFailed || state->level.levelComplete)
    return;

  Vector2 mouse = GetMousePosition();

  TryStartDrag(state, mouse);

  if (state->isDragging) {
    state->dragPos = mouse;
  }

  TryDropDraggedPiece(state);
}

/* ---- Game flow implementation ---- */

// ─── Game Init ───────────────────────────────────────────────────────────────
void GameInit(GameState *state)
{
    srand((unsigned int)time(NULL));

    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    BoardInit(&state->board);
    state->selectedLevel = 0;
    SaveLoad(&state->highScore, &state->unlockedLevel);

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        SlotClear(&state->slots[i]);
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

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, stdBtn)) {
                    SoundPlayMenuClick(&assets);
                    state->selectedLevel = 0;
                    state->currentScreen = SCREEN_PLAY;
                    GameReset(state);
                } else if (CheckCollisionPointRec(mouse, advBtn)) {
                    SoundPlayMenuClick(&assets);
                    state->currentScreen = SCREEN_LEVEL_SELECT;
                } else if (CheckCollisionPointRec(mouse, quitBtn)) {
                    SoundPlayMenuClick(&assets);
                    state->shouldQuit = true;
                } else if (CheckCollisionPointRec(mouse, gearRect)) {
                    SoundPlayMenuClick(&assets);
                    state->currentScreen = SCREEN_MENU_SETTINGS;
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
                    SaveWrite(state->highScore, state->unlockedLevel);
                }

                state->currentScreen = SCREEN_RESULT;
            }

            // Check if no valid moves (game over / level failed)
            if (LevelCheckFailure(&state->board, state->slots) &&
                !state->level.levelComplete) {
                state->level.levelFailed = true;

                if (state->selectedLevel == 0) {
                    // Classic mode: persist only the high score.
                    if (state->score > state->highScore)
                        state->highScore = state->score;
                    SaveWrite(state->highScore, state->unlockedLevel);

                    SoundPlayLose(&assets);
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
                    SoundPlayMenuClick(&assets);
                    state->currentScreen = SCREEN_MENU;
                    break;
                }

                for (int i = 0; i < TOTAL_LEVELS; i++) {
                    int row = i / LEVELS_PER_ROW;
                    int col = i % LEVELS_PER_ROW;
                    Rectangle btn = {100 + col * 95, 150 + row * 125, AMAP_BTN_SIZE, AMAP_BTN_SIZE};
                    if (CheckCollisionPointRec(mouse, btn)) {
                        if (LevelIsUnlocked(state->unlockedLevel, i + 1)) {
                            SoundPlayMenuClick(&assets);
                            state->selectedLevel = i + 1;  // levels are 1-indexed
                            state->currentScreen = SCREEN_PLAY;
                            GameReset(state);
                        }
                        break;
                    }
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&assets);
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
                        SoundPlayMenuClick(&assets);
                        state->level.levelFailed = false;
                        state->level.levelComplete = false;
                        GameReset(state);
                        state->currentScreen = SCREEN_PLAY;
                    } else if (CheckCollisionPointRec(mouse, btnBot)) {
                        SoundPlayMenuClick(&assets);
                        state->level.levelFailed = false;
                        state->level.levelComplete = false;
                        state->currentScreen = (state->selectedLevel > 0) ? SCREEN_LEVEL_SELECT : SCREEN_MENU;
                    }
                } else {
                    // Won: Top = Next Level, Bottom = Home
                    if (CheckCollisionPointRec(mouse, btnTop)) {
                        SoundPlayMenuClick(&assets);
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
                        SoundPlayMenuClick(&assets);
                        state->level.levelComplete = false;
                        state->currentScreen = SCREEN_LEVEL_SELECT;
                    }
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&assets);
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
    }
}


// Execute a setting action for the given index
void ExecuteSetting(GameState *state, int index)
{
    SoundPlayMenuClick(&assets);
    switch (index) {
        case SETTING_SFX:
            SoundToggleSfx(&assets);
            break;
        case SETTING_MUSIC:
            SoundToggleMusic(&assets);
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
    Rectangle sfxIcon = {160, 250, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE};
    Rectangle musicIcon = {280, 250, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE};
    Rectangle replayButton = {BTN_X, 330, BTN_W, BTN_H};
    Rectangle homeButton = {BTN_X, 395, BTN_W, BTN_H};

    if (CheckCollisionPointRec(mouse, sfxIcon)) state->selectedSetting = SETTING_SFX;
    else if (CheckCollisionPointRec(mouse, musicIcon)) state->selectedSetting = SETTING_MUSIC;
    else if (CheckCollisionPointRec(mouse, replayButton)) state->selectedSetting = SETTING_RESTART;
    else if (CheckCollisionPointRec(mouse, homeButton)) state->selectedSetting = SETTING_QUIT;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, sfxIcon)) ExecuteSetting(state, SETTING_SFX);
        else if (CheckCollisionPointRec(mouse, musicIcon)) ExecuteSetting(state, SETTING_MUSIC);
        else if (CheckCollisionPointRec(mouse, replayButton)) ExecuteSetting(state, SETTING_RESTART);
        else if (CheckCollisionPointRec(mouse, homeButton)) ExecuteSetting(state, SETTING_QUIT);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&assets);
        state->currentScreen = state->prevScreen;
    }
}


// Menu settings: SFX, Music, Reset Progress, Home (no Replay)
void GameUpdateMenuSettings(GameState *state)
{
    Vector2 mouse = GetMousePosition();
    Rectangle sfxIcon = {160, 250, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE};
    Rectangle musicIcon = {280, 250, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE};
    Rectangle resetButton = {BTN_X, 330, BTN_W, BTN_H};
    Rectangle homeButton = {BTN_X, 395, BTN_W, BTN_H};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, sfxIcon)) {
            SoundPlayMenuClick(&assets);
            SoundToggleSfx(&assets);
        } else if (CheckCollisionPointRec(mouse, musicIcon)) {
            SoundPlayMenuClick(&assets);
            SoundToggleMusic(&assets);
        } else if (CheckCollisionPointRec(mouse, resetButton)) {
            SoundPlayMenuClick(&assets);
            state->unlockedLevel = 1;
            state->highScore = 0;
            SaveWrite(state->highScore, state->unlockedLevel);
        } else if (CheckCollisionPointRec(mouse, homeButton)) {
            SoundPlayMenuClick(&assets);
            state->currentScreen = SCREEN_MENU;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        SoundPlayMenuClick(&assets);
        state->currentScreen = SCREEN_MENU;
    }
}
