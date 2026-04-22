#include "render.h"
#include "score.h"
#include "float_text.h"
#include "particle.h"
#include <stdio.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

// Color palette for pieces (index 0 = empty/background)
static const Color PIECE_COLORS[] = {
    {40, 40, 60, 255},             // 0 = empty cell background
    {255, 87, 87, 255},            // 1 = red
    {255, 189, 46, 255},           // 2 = yellow
    {46, 204, 113, 255},           // 3 = green
    {52, 152, 219, 255},           // 4 = blue
    {155, 89, 182, 255},           // 5 = purple
    {255, 147, 51, 255},           // 6 = orange
    {26, 188, 156, 255},           // 7 = teal
};

#define BG_COLOR       (Color){22, 22, 36, 255}
#define GRID_BG_COLOR  (Color){30, 30, 48, 255}
#define GRID_LINE_CLR  (Color){50, 50, 70, 255}
#define PANEL_PIECE_SCALE 25

// Settings menu constants
#define SETTING_COUNT 4

// --- Forward declarations ---
static void RenderBoard(GameState *state);
static void RenderPieceSlots(GameState *state);
static void RenderDraggedPiece(GameState *state);
static void RenderGhost(GameState *state);
static void RenderScore(GameState *state);
static void RenderBanner(GameState *state);
static void RenderMenu(GameState *state);
static void RenderSettings(GameState *state);
static void RenderGameOver(GameState *state);

void RenderFrame(GameState *state)
{
    BeginDrawing();
    ClearBackground(BG_COLOR);

    switch (state->currentScreen) {
        case SCREEN_MENU:
            RenderMenu(state);
            break;
        case SCREEN_PLAY:
            RenderScore(state);
            RenderBoard(state);
            RenderGhost(state);
            RenderPieceSlots(state);
            RenderDraggedPiece(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            RenderBanner(state);
            break;
        case SCREEN_SETTINGS:
            // Draw the game behind the overlay
            RenderScore(state);
            RenderBoard(state);
            RenderPieceSlots(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            RenderBanner(state);
            // Settings overlay on top
            RenderSettings(state);
            break;
        case SCREEN_GAMEOVER:
            RenderScore(state);
            RenderBoard(state);
            ParticleDraw(&state->particles);
            RenderGameOver(state);
            break;
    }

    EndDrawing();
}

// ----- Board -----
static void RenderBoard(GameState *state)
{
    // Draw board background
    DrawRectangle(GRID_DRAW_X - 4, GRID_DRAW_Y - 4,
                  GRID_SIZE * CELL_SIZE + 8, GRID_SIZE * CELL_SIZE + 8,
                  GRID_LINE_CLR);

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            int x = GRID_DRAW_X + c * CELL_SIZE;
            int y = GRID_DRAW_Y + r * CELL_SIZE;
            int val = state->board.cells[r][c];

            // Check if cell is being animated
            float alpha = AnimGetCellAlpha(&state->anims, r, c);
            if (alpha >= 0.0f) {
                // Animated cell: fade out
                Color clr = PIECE_COLORS[val > 0 ? val : 1];
                clr.a = (unsigned char)(alpha * 255);
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
            } else if (val != CELL_EMPTY) {
                // Normal filled cell
                Color clr = PIECE_COLORS[val];
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
                // Highlight edge
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, 3,
                    (Color){255,255,255,40});
            } else {
                // Empty cell
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2,
                    PIECE_COLORS[0]);
            }
        }
    }
}

// ----- Ghost preview -----
static void RenderGhost(GameState *state)
{
    if (!state->isDragging) return;

    PieceSlot *slot = &state->slots[state->dragSlotIndex];
    Piece *piece = slot->piece;
    if (!piece) return;

    float pieceScreenX = state->dragPos.x - state->dragOffset.x;
    float pieceScreenY = state->dragPos.y - state->dragOffset.y;

    int gridCol = (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE/2) / CELL_SIZE);
    int gridRow = (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE/2) / CELL_SIZE);

    if (!BoardCanPlace(&state->board, piece, gridRow, gridCol)) return;

    Color ghost = PIECE_COLORS[piece->colorIndex];
    ghost.a = 80;

    for (int r = 0; r < piece->height; r++) {
        for (int c = 0; c < piece->width; c++) {
            if (piece->shape[r][c] == 0) continue;
            int x = GRID_DRAW_X + (gridCol + c) * CELL_SIZE;
            int y = GRID_DRAW_Y + (gridRow + r) * CELL_SIZE;
            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, ghost);
        }
    }
}

// ----- Piece slots (bottom panel) -----
static void RenderPieceSlots(GameState *state)
{
    // Panel background
    DrawRectangle(0, PANEL_Y - 10, SCREEN_WIDTH, SCREEN_HEIGHT - PANEL_Y + 10,
        (Color){30, 30, 50, 255});
    DrawRectangle(0, PANEL_Y - 10, SCREEN_WIDTH, 2, GRID_LINE_CLR);

    for (int i = 0; i < 3; i++) {
        // Skip the slot being dragged
        if (state->isDragging && state->dragSlotIndex == i) continue;

        PieceSlot *slot = &state->slots[i];
        if (!slot->piece) continue;

        Piece *p = slot->piece;
        Color clr = PIECE_COLORS[p->colorIndex];

        for (int r = 0; r < p->height; r++) {
            for (int c = 0; c < p->width; c++) {
                if (p->shape[r][c] == 0) continue;
                int x = (int)(slot->posX + c * PANEL_PIECE_SCALE);
                int y = (int)(slot->posY + r * PANEL_PIECE_SCALE);
                DrawRectangle(x + 1, y + 1,
                    PANEL_PIECE_SCALE - 2, PANEL_PIECE_SCALE - 2, clr);
            }
        }
    }
}

// ----- Dragged piece (follows mouse at grid scale) -----
static void RenderDraggedPiece(GameState *state)
{
    if (!state->isDragging) return;

    PieceSlot *slot = &state->slots[state->dragSlotIndex];
    Piece *p = slot->piece;
    if (!p) return;

    float baseX = state->dragPos.x - state->dragOffset.x;
    float baseY = state->dragPos.y - state->dragOffset.y;

    Color clr = PIECE_COLORS[p->colorIndex];
    clr.a = 200;

    for (int r = 0; r < p->height; r++) {
        for (int c = 0; c < p->width; c++) {
            if (p->shape[r][c] == 0) continue;
            int x = (int)(baseX + c * CELL_SIZE);
            int y = (int)(baseY + r * CELL_SIZE);
            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
        }
    }
}

// ----- Score display -----
static void RenderScore(GameState *state)
{
    char buf[64];

    // Score
    sprintf(buf, "SKOR: %d", state->score);
    int w = MeasureText(buf, 28);
    DrawText(buf, (SCREEN_WIDTH - w) / 2, 20, 28, WHITE);

    // High score
    sprintf(buf, "EN YUKSEK: %d", state->highScore);
    w = MeasureText(buf, 18);
    DrawText(buf, (SCREEN_WIDTH - w) / 2, 55, 18, (Color){150,150,170,255});

    // Combo
    if (state->combo > 1) {
        sprintf(buf, "COMBO x%d", state->combo);
        w = MeasureText(buf, 22);
        DrawText(buf, (SCREEN_WIDTH - w) / 2, 85, 22,
            (Color){255, 220, 50, 255});
    }
}

// ----- Best Combo banner -----
static void RenderBanner(GameState *state)
{
    if (!state->banner.active) return;

    float t = state->banner.timer / state->banner.duration; // 1.0 -> 0.0
    float progress = 1.0f - t; // 0.0 -> 1.0

    // Pulse effect on font size
    int baseSize = 40;
    int pulseSize = baseSize + (int)(8.0f * sinf(progress * PI * 4.0f));

    // Alpha: fade in quickly, stay, fade out at the end
    float alpha;
    if (progress < 0.15f) {
        alpha = progress / 0.15f;          // fade in
    } else if (progress > 0.7f) {
        alpha = (1.0f - progress) / 0.3f;  // fade out
    } else {
        alpha = 1.0f;                       // fully visible
    }

    Color clr = {255, 220, 50, (unsigned char)(alpha * 255.0f)};
    Color shadow = {0, 0, 0, (unsigned char)(alpha * 120.0f)};

    int w = MeasureText(state->banner.text, pulseSize);
    int x = (SCREEN_WIDTH - w) / 2;
    int y = 100;

    // Drop shadow
    DrawText(state->banner.text, x + 2, y + 2, pulseSize, shadow);
    // Main text
    DrawText(state->banner.text, x, y, pulseSize, clr);
}

// ----- Menu screen -----
static void RenderMenu(GameState *state)
{
    const char *title = "BLOCK BLAST";
    int tw = MeasureText(title, 48);
    DrawText(title, (SCREEN_WIDTH - tw)/2, 200, 48, WHITE);

    const char *sub = "Baslamak icin ENTER'a bas";
    int sw = MeasureText(sub, 20);
    DrawText(sub, (SCREEN_WIDTH - sw)/2, 400, 20,
        (Color){150,150,170,255});

    char buf[64];
    sprintf(buf, "En Yuksek Skor: %d", state->highScore);
    int hw = MeasureText(buf, 20);
    DrawText(buf, (SCREEN_WIDTH - hw)/2, 320, 20,
        (Color){100,100,130,255});
}

// ----- Settings screen (overlay) -----
static void RenderSettings(GameState *state)
{
    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

    // Title
    const char *title = "AYARLAR";
    int tw = MeasureText(title, 36);
    DrawText(title, (SCREEN_WIDTH - tw) / 2, 200, 36, WHITE);

    // Separator line
    DrawRectangle(SCREEN_WIDTH / 2 - 80, 245, 160, 2, (Color){80, 80, 100, 255});

    // Build settings labels dynamically based on sound state
    char sfxLabel[32];
    char musicLabel[32];
    sprintf(sfxLabel, "Ses Efektleri: %s", state->sound.sfxEnabled ? "ACIK" : "KAPALI");
    sprintf(musicLabel, "Muzik: %s", state->sound.musicEnabled ? "ACIK" : "KAPALI");

    const char *items[SETTING_COUNT] = {
        sfxLabel,
        musicLabel,
        "Yeniden Baslat",
        "Ana Menuye Don"
    };

    for (int i = 0; i < SETTING_COUNT; i++) {
        int fontSize = 24;
        Color clr;

        if (i == state->selectedSetting) {
            // Selected item: golden with indicator
            clr = (Color){255, 220, 50, 255};
            const char *indicator = "> ";
            int fullW = MeasureText(indicator, fontSize) + MeasureText(items[i], fontSize);
            int x = (SCREEN_WIDTH - fullW) / 2;
            int y = 270 + i * 55;
            DrawText(indicator, x, y, fontSize, clr);
            DrawText(items[i], x + MeasureText(indicator, fontSize), y, fontSize, clr);
        } else {
            clr = (Color){180, 180, 200, 255};
            int w = MeasureText(items[i], fontSize);
            DrawText(items[i], (SCREEN_WIDTH - w) / 2, 270 + i * 55, fontSize, clr);
        }
    }

    // Footer hint
    const char *hint = "ESC: Geri Don  |  ENTER: Sec";
    int hw = MeasureText(hint, 16);
    DrawText(hint, (SCREEN_WIDTH - hw) / 2, 510, 16, (Color){100, 100, 120, 255});
}

// ----- Game Over screen -----
static void RenderGameOver(GameState *state)
{
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,160});

    const char *go = "OYUN BITTI";
    int gw = MeasureText(go, 44);
    DrawText(go, (SCREEN_WIDTH - gw)/2, 280, 44, (Color){255,87,87,255});

    char buf[64];
    sprintf(buf, "Skor: %d", state->score);
    int sw = MeasureText(buf, 28);
    DrawText(buf, (SCREEN_WIDTH - sw)/2, 350, 28, WHITE);

    sprintf(buf, "En Yuksek: %d", state->highScore);
    sw = MeasureText(buf, 22);
    DrawText(buf, (SCREEN_WIDTH - sw)/2, 395, 22, (Color){150,150,170,255});

    const char *retry = "Tekrar oynamak icin ENTER'a bas";
    int rw = MeasureText(retry, 18);
    DrawText(retry, (SCREEN_WIDTH - rw)/2, 460, 18,
        (Color){100,100,130,255});
}
