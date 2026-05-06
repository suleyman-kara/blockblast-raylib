#ifndef THEME_H
#define THEME_H

#include "raylib.h"

// ═══════════════════════════════════════════════════════════════════════════════
//  THEME — Central style palette for the entire game
//  ═══════════════════════════════════════════════════════════════════════════════
//  All visual constants — colors, fonts, sizes, borders — in one place.
//  Every render function should read from these values so that the whole
//  game can be re-skinned by editing this single file.
// ═══════════════════════════════════════════════════════════════════════════════

// ─── Button styling ──────────────────────────────────────────────────────────
typedef struct {
    Color bg;
    Color bgHover;
    Color border;
    Color borderHover;
    Color text;
    Color textHover;
    float cornerRadius;
    int borderSegments;     // DrawRectangleRounded segments
} ButtonStyle;

// ─── Text styling ────────────────────────────────────────────────────────────
typedef struct {
    Color primary;
    Color secondary;
    Color muted;
    int headingSize;        // e.g. 36
    int bodySize;           // e.g. 22
    int smallSize;          // e.g. 14
} TextStyle;

// ─── Grid & Board ────────────────────────────────────────────────────────────
typedef struct {
    Color background;
    Color border;
    Color emptyCell;
    Color iceOverlay;
    Color stoneBase;
    Color stoneMid;
    Color stoneDark;
    Color highlight;        // top-edge highlight on filled cells
    int cellSize;
} GridStyle;

// ─── Gem (Elmas / Zümrüt) ───────────────────────────────────────────────────
typedef struct {
    Color diamondPrimary;
    Color diamondHighlight;
    Color emeraldPrimary;
    Color emeraldHighlight;
    float sizeRatio;        // what fraction of CELL_SIZE the gem icon occupies
} GemStyle;

// ─── Panel (bottom piece slot area) ─────────────────────────────────────────
typedef struct {
    Color background;
    Color topBorder;
    float topBorderWidth;
} PanelStyle;

// ─── Effects (animations, particles, banners) ───────────────────────────────
typedef struct {
    Color bannerText;
    Color bannerShadow;
    Color floatText;
    Color particleColors[7];    // one per piece color index (1-7)
    int bannerPulseBaseSize;
} EffectStyle;

// ─── Settings overlay ───────────────────────────────────────────────────────
typedef struct {
    Color overlayBg;
    Color cardBg;
    Color cardBorder;
    Color separator;
    Color selectedText;
    Color unselectedText;
    Color footerHint;
    int cardWidth;
    int cardHeight;
    int titleFontSize;
    int itemFontSize;
    int itemSpacing;
    int paddingX;
    int paddingTop;
} SettingsStyle;

// ─── Adventure Map ──────────────────────────────────────────────────────────
typedef struct {
    Color lockedBg;
    Color lockedBorder;
    Color unlockedBg;
    Color unlockedBgHover;
    Color unlockedBorder;
    Color completedText;
    Color lockedNumber;
    Color starFilled;
    Color starEmpty;
    int btnSize;
    int btnGap;
    int btnLabelGap;
    int mapStartY;
} AdventureMapStyle;

// ─── Master theme ───────────────────────────────────────────────────────────
typedef struct {
    ButtonStyle menuStandard;
    ButtonStyle menuAdventure;
    TextStyle   text;
    GridStyle   grid;
    GemStyle    gem;
    PanelStyle  panel;
    EffectStyle effects;
    SettingsStyle settings;
    AdventureMapStyle adventureMap;
} Theme;

// ─── Default theme instance ─────────────────────────────────────────────────
static const Theme THEME_DEFAULT = {
    // Menu: Standard Mode button (lighter blue)
    .menuStandard = {
        .bg          = { 60,  100, 180, 255 },
        .bgHover     = { 80,  130, 210, 255 },
        .border      = { 100, 150, 220, 255 },
        .borderHover = { 140, 200, 255, 255 },
        .text        = { 220, 230, 250, 255 },
        .textHover   = { 255, 255, 255, 255 },
        .cornerRadius = 0.25f,
        .borderSegments = 8,
    },
    // Menu: Adventure Mode button
    .menuAdventure = {
        .bg          = { 55,  40,  85, 255 },
        .bgHover     = { 80,  60, 120, 255 },
        .border      = { 100, 70, 150, 255 },
        .borderHover = { 180, 120, 255, 255 },
        .text        = { 170, 150, 200, 255 },
        .textHover   = { 230, 210, 255, 255 },
        .cornerRadius = 0.25f,
        .borderSegments = 8,
    },
    // Text
    .text = {
        .primary   = { 255, 255, 255, 255 },
        .secondary = { 150, 150, 170, 255 },
        .muted     = { 100, 100, 140, 200 },
        .headingSize = 36,
        .bodySize    = 22,
        .smallSize   = 14,
    },
    // Grid & Board
    .grid = {
        .background    = { 30,  30,  48, 255 },
        .border        = { 50,  50,  70, 255 },
        .emptyCell     = { 40,  40,  60, 255 },
        .iceOverlay    = { 200, 230, 255, 200 },
        .stoneBase     = { 80,  80,  90, 255 },
        .stoneMid      = { 90,  90, 100, 255 },
        .stoneDark     = { 70,  70,  80, 255 },
        .highlight     = { 255, 255, 255, 40 },
        .cellSize      = 50,
    },
    // Gems (diamond / emerald)
    .gem = {
        .diamondPrimary    = { 100, 200, 255, 255 },
        .diamondHighlight  = { 255, 255, 255, 200 },
        .emeraldPrimary    = { 50,  220, 100, 255 },
        .emeraldHighlight  = { 200, 255, 220, 200 },
        .sizeRatio         = 0.50f,
    },
    // Bottom panel
    .panel = {
        .background     = { 30,  30,  50, 255 },
        .topBorder      = { 50,  50,  70, 255 },
        .topBorderWidth = 2.0f,
    },
    // Effects
    .effects = {
        .bannerText        = { 255, 220, 50, 255 },
        .bannerShadow      = { 0,   0,   0, 120 },
        .floatText         = { 255, 255, 100, 255 },
        .particleColors    = {
            { 255, 87,  87,  255 },   // index 1 — red
            { 255, 189, 46,  255 },   // index 2 — yellow
            { 46,  204, 113, 255 },   // index 3 — green
            { 52,  152, 219, 255 },   // index 4 — blue
            { 155, 89,  182, 255 },   // index 5 — purple
            { 255, 147, 51,  255 },   // index 6 — orange
            { 26,  188, 156, 255 },   // index 7 — teal
        },
        .bannerPulseBaseSize = 40,
    },
    // Settings overlay
    .settings = {
        .overlayBg      = { 0,   0,   0, 160 },
        .cardBg         = { 20,  25,  50, 235 },
        .cardBorder     = { 60,  70, 110, 180 },
        .separator      = { 80,  80, 120, 200 },
        .selectedText   = { 255, 220, 50, 255 },
        .unselectedText = { 180, 180, 210, 255 },
        .footerHint     = { 100, 100, 140, 200 },
        .cardWidth      = 320,
        .cardHeight     = 400,
        .titleFontSize  = 30,
        .itemFontSize   = 22,
        .itemSpacing    = 55,
        .paddingX       = 40,     // left padding for items
        .paddingTop     = 90,     // top offset of first item within card
    },
    // Adventure Map
    .adventureMap = {
        .lockedBg         = { 25,  25,  40, 255 },
        .lockedBorder     = { 40,  40,  55, 255 },
        .unlockedBg       = { 40,  35,  60, 255 },
        .unlockedBgHover  = { 60,  50,  80, 255 },
        .unlockedBorder   = { 120, 80, 180, 255 },
        .completedText    = { 50,  255, 100, 255 },
        .lockedNumber     = { 60,  60,  80, 255 },
        .starFilled       = { 255, 220, 50, 255 },
        .starEmpty        = { 60,  60,  80, 100 },
        .btnSize    = 80,
        .btnGap     = 15,
        .btnLabelGap = 30,   // vertical gap between button and its label
        .mapStartY  = 150,
    },
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Piece Colors (index 0 = empty, 1-7 = piece colors)
//  ═══════════════════════════════════════════════════════════════════════════════
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

// Background color for the whole screen
#define BG_COLOR            ((Color){ 40,  60,  100, 255 })

#endif // THEME_H