#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════════════════
//  BLOCK BLAST — Configuration Constants
//  ═══════════════════════════════════════════════════════════════════════════════
//  All tunable game constants in one place. Change these to quickly adjust
//  grid size, colors, layout, scoring, etc.
// ═══════════════════════════════════════════════════════════════════════════════

// ─── Screen & Layout ──────────────────────────────────────────────────────────
#define SCREEN_WIDTH       480
#define SCREEN_HEIGHT      800
#define CELL_SIZE          50
#define GRID_DRAW_X        40    // (SCREEN_WIDTH - GRID_SIZE * CELL_SIZE) / 2
#define GRID_DRAW_Y        130
#define PANEL_Y            580   // Y position of the bottom piece panel

// ─── Grid ─────────────────────────────────────────────────────────────────────
#define GRID_SIZE          8
#define CELL_EMPTY         0
#define CELL_ICE           8
#define CELL_STONE         9

// ─── Piece System ─────────────────────────────────────────────────────────────
#define MAX_PIECE_SIZE     5
#define PIECE_DEF_COUNT    18
#define PANEL_PIECE_SCALE  25    // piece cell size in bottom panel
#define DRAG_PIECE_SCALE   CELL_SIZE

// ─── Scoring ──────────────────────────────────────────────────────────────────
#define BASE_SCORE         100

// ─── Banner ───────────────────────────────────────────────────────────────────
#define BANNER_DURATION    1.8f

// ─── Animation ────────────────────────────────────────────────────────────────
#define CLEAR_ANIM_DURATION  0.35f
#define MAX_ANIMS            16

// ─── Floating Text ────────────────────────────────────────────────────────────
#define MAX_FLOAT_TEXTS      8
#define FLOAT_TEXT_LEN       32
#define FLOAT_TEXT_DURATION  1.2f
#define FLOAT_TEXT_SPEED     60.0f   // pixels per second upward

// ─── Particles ────────────────────────────────────────────────────────────────
#define MAX_PARTICLES        512
#define PARTICLE_LIFE        0.7f

// ─── Adventure Mode ───────────────────────────────────────────────────────────
#define MAX_LIVES            5
#define LIFE_REGEN_SEC       (20 * 60)   // 20 minutes per life
#define STAR_1_THRESHOLD     1.00f       // 100% of goal = 1 star
#define STAR_2_THRESHOLD     1.50f       // 150% of goal = 2 stars
#define STAR_3_THRESHOLD     2.00f       // 200% of goal = 3 stars
#define TOTAL_LEVELS         15
#define LEVELS_PER_ROW       3
#define MAX_OBSTACLES        9

// ─── Settings Menu ────────────────────────────────────────────────────────────
#define SETTING_COUNT        4
#define SETTING_SFX          0
#define SETTING_MUSIC        1
#define SETTING_RESTART      2
#define SETTING_QUIT         3

// ─── Gear Icon (settings button) ──────────────────────────────────────────────
#define GEAR_X               (SCREEN_WIDTH - 50)
#define GEAR_Y               10
#define GEAR_SIZE            36

// ─── Menu Buttons ─────────────────────────────────────────────────────────────
#define MENU_BTN_W           260
#define MENU_BTN_H           55
#define MENU_BTN_X           ((SCREEN_WIDTH - MENU_BTN_W) / 2)
#define MENU_STD_Y           340
#define MENU_ADV_Y           420

// ═══════════════════════════════════════════════════════════════════════════════
//  Color Palette
//  ═══════════════════════════════════════════════════════════════════════════════
//  Index 0 = empty cell background, 1-7 = piece colors, 8-9 = ice/stone
//  Change these to quickly alter the game's visual theme.
// ═══════════════════════════════════════════════════════════════════════════════

static const Color PIECE_COLORS[] = {
    { 40,  40,  60,  255 },   // 0 = empty cell background
    { 255, 87,  87,  255 },   // 1 = red
    { 255, 189, 46,  255 },   // 2 = yellow
    { 46,  204, 113, 255 },   // 3 = green
    { 52,  152, 219, 255 },   // 4 = blue
    { 155, 89,  182, 255 },   // 5 = purple
    { 255, 147, 51,  255 },   // 6 = orange
    { 26,  188, 156, 255 },   // 7 = teal
};

// Theme colors
#define BG_COLOR            ((Color){ 22,  22,  36,  255 })
#define GRID_BG_COLOR       ((Color){ 30,  30,  48,  255 })
#define GRID_LINE_CLR       ((Color){ 50,  50,  70,  255 })
#define ICE_COLOR           ((Color){ 200, 230, 255, 200 })
#define STONE_COLOR         ((Color){ 80,  80,  90,  255 })

#endif // CONFIG_H