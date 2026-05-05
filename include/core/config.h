#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"
#include "theme.h"
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════════════════
//  BLOCK BLAST — Configuration Constants
//  ═══════════════════════════════════════════════════════════════════════════════
//  All tunable game constants in one place.
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

// Gem types stored in the gems[][] array
#define GEM_NONE           0
#define GEM_DIAMOND        1
#define GEM_EMERALD        2

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
#define TOTAL_LEVELS         10
#define LEVELS_PER_ROW       3
#define MAX_OBSTACLES        9

// Gem spawn probability (0.0 – 1.0) for new pieces in gem levels
#define DIAMOND_SPAWN_CHANCE  0.15f
#define EMERALD_SPAWN_CHANCE  0.12f

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
#define MENU_BTN_W           300
#define MENU_BTN_H           65
#define MENU_BTN_X           ((SCREEN_WIDTH - MENU_BTN_W) / 2)
#define MENU_STD_Y           360
#define MENU_ADV_Y           450

// ═══════════════════════════════════════════════════════════════════════════════
//  Piece Colors
//  ═══════════════════════════════════════════════════════════════════════════════
//  Moved to theme.h — kept here as alias for backward compatibility
// ═══════════════════════════════════════════════════════════════════════════════

#endif // CONFIG_H