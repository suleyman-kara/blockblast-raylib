#ifndef DEFS_H
#define DEFS_H

#include "raylib.h"
#include <math.h>
#include <stdbool.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

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

// ─── Banner
// ───────────────────────────────────────────────────────────────────
#define BANNER_DURATION 1.8f

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
#define SETTING_COUNT 4
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
#define MENU_STD_Y 360
#define MENU_ADV_Y 450

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
static const Color PIECE_COLORS[] = {
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
#define SETTINGS_CARD_HEIGHT 400
#define SETTINGS_TITLE_FONT_SIZE 30
#define SETTINGS_CARD_PADDING_X 40

// ─── Gems
// ─────────────────────────────────────────────────────────────────────
#define GEM_SIZE_RATIO 0.50f

#endif // DEFS_H