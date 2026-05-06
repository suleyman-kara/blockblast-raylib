#ifndef DEFS_H
#define DEFS_H

#include "raylib.h"
#include <stdbool.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  BLOCK BLAST — Single file with all constants and colours
//  ═══════════════════════════════════════════════════════════════════════════════

// ─── Screen & Layout ──────────────────────────────────────────────────────────
#define SCREEN_WIDTH       480
#define SCREEN_HEIGHT      800
#define CELL_SIZE          50
#define GRID_DRAW_X        40
#define GRID_DRAW_Y        130
#define PANEL_Y            580

// ─── Grid ─────────────────────────────────────────────────────────────────────
#define GRID_SIZE          8
#define CELL_EMPTY         0
#define CELL_ICE           8
#define CELL_STONE         9
#define GEM_NONE           0
#define GEM_DIAMOND        1
#define GEM_EMERALD        2

// ─── Piece System ─────────────────────────────────────────────────────────────
#define MAX_PIECE_SIZE     5
#define PIECE_DEF_COUNT    18
#define PANEL_PIECE_SCALE  25
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
#define FLOAT_TEXT_SPEED     60.0f

// ─── Particles ────────────────────────────────────────────────────────────────
#define MAX_PARTICLES        512
#define PARTICLE_LIFE        0.7f

// ─── Adventure Mode ───────────────────────────────────────────────────────────
#define TOTAL_LEVELS         10
#define LEVELS_PER_ROW       3
#define MAX_OBSTACLES        9
#define DIAMOND_SPAWN_CHANCE  0.15f
#define EMERALD_SPAWN_CHANCE  0.12f

// ─── Settings Menu ────────────────────────────────────────────────────────────
#define SETTING_COUNT        4
#define SETTING_SFX          0
#define SETTING_MUSIC        1
#define SETTING_RESTART      2
#define SETTING_QUIT         3
#define SETTINGS_ICON_SIZE   40
#define SETTINGS_ICON_GAP    60
#define SETTINGS_LABEL_OFFSET 50
#define SETTINGS_BTN_W       200
#define SETTINGS_BTN_H       45
#define SETTINGS_BTN_GAP     15

// ─── Gear Icon ────────────────────────────────────────────────────────────────
#define GEAR_X               (SCREEN_WIDTH - 50)
#define GEAR_Y               10
#define GEAR_SIZE            36

// ─── Menu Buttons ─────────────────────────────────────────────────────────────
#define MENU_BTN_W           300
#define MENU_BTN_H           65
#define MENU_BTN_X           ((SCREEN_WIDTH - MENU_BTN_W) / 2)
#define MENU_STD_Y           360
#define MENU_ADV_Y           450

// ─── Title & Subtitle ─────────────────────────────────────────────────────────
#define TITLE_FONT_SIZE      65
#define SUBTITLE_FONT_SIZE   32

// ─── Bottom Zone Dragging ─────────────────────────────────────────────────────
#define BOTTOM_ZONE_Y        PANEL_Y
#define BOTTOM_ZONE_HEIGHT   (SCREEN_HEIGHT - PANEL_Y)
#define DRAG_SPEED_BASE      1.0f
#define DRAG_SPEED_FACTOR    1.0f

// ═══════════════════════════════════════════════════════════════════════════════
//  Colour palette — flat constants, no structs
//  ═══════════════════════════════════════════════════════════════════════════════

// Background colour for the whole screen
#define COLOR_BG                ((Color){ 40,  60,  100, 255 })
#define BG_COLOR                COLOR_BG   // backward compatibility

// ─── Piece colours (index 0 = empty, 1-7 = piece colours) ────────────────────
static const Color PIECE_COLORS[] = {
    { 40,  40,  60,  255 },   // 0 = empty cell
    { 255, 87,  87,  255 },   // 1 = red
    { 255, 189, 46,  255 },   // 2 = yellow
    { 46,  204, 113, 255 },   // 3 = green
    { 52,  152, 219, 255 },   // 4 = blue
    { 155, 89,  182, 255 },   // 5 = purple
    { 255, 147, 51,  255 },   // 6 = orange
    { 26,  188, 156, 255 },   // 7 = teal
};

// ─── Text ─────────────────────────────────────────────────────────────────────
#define COLOR_TEXT_PRIMARY      ((Color){ 255, 255, 255, 255 })
#define COLOR_TEXT_SECONDARY    ((Color){ 150, 150, 170, 255 })
#define COLOR_TEXT_MUTED        ((Color){ 100, 100, 140, 200 })

// ─── Grid & Board ─────────────────────────────────────────────────────────────
#define COLOR_GRID_BORDER       ((Color){ 50,  50,  70, 255 })
#define COLOR_GRID_EMPTY        ((Color){ 40,  40,  60, 255 })
#define COLOR_GRID_ICE_OVERLAY  ((Color){ 200, 230, 255, 200 })
#define COLOR_GRID_HIGHLIGHT    ((Color){ 255, 255, 255, 40  })
#define COLOR_STONE_BASE        ((Color){ 80,  80,  90, 255 })
#define COLOR_STONE_MID         ((Color){ 90,  90,  100, 255 })
#define COLOR_STONE_DARK        ((Color){ 70,  70,  80, 255 })

// ─── Panel (bottom piece slot area) ───────────────────────────────────────────
#define COLOR_PANEL_BG          ((Color){ 30,  30,  50, 255 })
#define COLOR_PANEL_TOP_BORDER  ((Color){ 50,  50,  70, 255 })
#define PANEL_TOP_BORDER_WIDTH  2.0f

// ─── Settings overlay ─────────────────────────────────────────────────────────
#define COLOR_SETTINGS_OVERLAY_BG       ((Color){ 0,   0,   0,   160 })
#define COLOR_SETTINGS_CARD_BG          ((Color){ 20,  25,  50,  235 })
#define COLOR_SETTINGS_CARD_BORDER      ((Color){ 60,  70,  110, 180 })
#define COLOR_SETTINGS_SEPARATOR        ((Color){ 80,  80,  120, 200 })
#define COLOR_SETTINGS_SELECTED_TEXT    ((Color){ 255, 220, 50,  255 })
#define COLOR_SETTINGS_UNSELECTED_TEXT  ((Color){ 180, 180, 210, 255 })
#define COLOR_SETTINGS_FOOTER_HINT      ((Color){ 100, 100, 140, 200 })
#define SETTINGS_CARD_WIDTH      320
#define SETTINGS_CARD_HEIGHT     400
#define SETTINGS_TITLE_FONT_SIZE 30
#define SETTINGS_ITEM_FONT_SIZE  22
#define SETTINGS_ITEM_SPACING    55
#define SETTINGS_CARD_PADDING_X  40
#define SETTINGS_CARD_PADDING_TOP 90

// ─── Effects ──────────────────────────────────────────────────────────────────
#define COLOR_BANNER_TEXT       ((Color){ 255, 220, 50,  255 })
#define COLOR_BANNER_SHADOW     ((Color){ 0,   0,   0,   120 })
#define COLOR_FLOAT_TEXT        ((Color){ 255, 255, 100, 255 })
#define BANNER_PULSE_BASE_SIZE  40

// ─── Menu Buttons — Classic Mode ──────────────────────────────────────────────
#define COLOR_BTN_STD_BG            ((Color){ 60,  100, 180, 255 })
#define COLOR_BTN_STD_BG_HOVER      ((Color){ 80,  130, 210, 255 })
#define COLOR_BTN_STD_BORDER        ((Color){ 100, 150, 220, 255 })
#define COLOR_BTN_STD_BORDER_HOVER  ((Color){ 140, 200, 255, 255 })
#define COLOR_BTN_STD_TEXT          ((Color){ 220, 230, 250, 255 })
#define COLOR_BTN_STD_TEXT_HOVER    ((Color){ 255, 255, 255, 255 })
#define BTN_STD_CORNER_RADIUS       0.25f
#define BTN_STD_BORDER_SEGMENTS     8

// ─── Menu Buttons — Adventure Mode ────────────────────────────────────────────
#define COLOR_BTN_ADV_BG            ((Color){ 55,  40,  85, 255 })
#define COLOR_BTN_ADV_BG_HOVER      ((Color){ 80,  60,  120, 255 })
#define COLOR_BTN_ADV_BORDER        ((Color){ 100, 70,  150, 255 })
#define COLOR_BTN_ADV_BORDER_HOVER  ((Color){ 180, 120, 255, 255 })
#define COLOR_BTN_ADV_TEXT          ((Color){ 170, 150, 200, 255 })
#define COLOR_BTN_ADV_TEXT_HOVER    ((Color){ 230, 210, 255, 255 })
#define BTN_ADV_CORNER_RADIUS       0.25f
#define BTN_ADV_BORDER_SEGMENTS     8

// ─── Gems ─────────────────────────────────────────────────────────────────────
#define COLOR_GEM_DIAMOND_PRIMARY     ((Color){ 100, 200, 255, 255 })
#define COLOR_GEM_DIAMOND_HIGHLIGHT   ((Color){ 255, 255, 255, 200 })
#define COLOR_GEM_EMERALD_PRIMARY     ((Color){ 50,  220, 100, 255 })
#define COLOR_GEM_EMERALD_HIGHLIGHT   ((Color){ 200, 255, 220, 200 })
#define GEM_SIZE_RATIO                0.50f

// ─── Adventure Map ────────────────────────────────────────────────────────────
#define COLOR_AMAP_LOCKED_BG       ((Color){ 25,  25,  40,  255 })
#define COLOR_AMAP_LOCKED_BORDER   ((Color){ 40,  40,  55,  255 })
#define COLOR_AMAP_UNLOCKED_BG     ((Color){ 40,  35,  60,  255 })
#define COLOR_AMAP_UNLOCKED_HOVER  ((Color){ 60,  50,  80,  255 })
#define COLOR_AMAP_UNLOCKED_BORDER ((Color){ 120, 80,  180, 255 })
#define COLOR_AMAP_COMPLETED_TEXT  ((Color){ 50,  255, 100, 255 })
#define COLOR_AMAP_LOCKED_NUMBER   ((Color){ 60,  60,  80,  255 })
#define COLOR_AMAP_STAR_FILLED     ((Color){ 255, 220, 50,  255 })
#define COLOR_AMAP_STAR_EMPTY      ((Color){ 60,  60,  80,  100 })
#define AMAP_BTN_SIZE              80
#define AMAP_BTN_GAP               15
#define AMAP_BTN_LABEL_GAP         30
#define AMAP_START_Y               150

#endif // DEFS_H