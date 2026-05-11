#include "render.h"
#include "defs.h"

// ─── Grid & Board Colors ────────────────────────────────────────────────────────
#define COLOR_GRID_BORDER       ((Color){ 50,  50,  70, 255 })
#define COLOR_GRID_EMPTY        ((Color){ 40,  40,  60, 255 })
#define COLOR_GRID_HIGHLIGHT    ((Color){ 255, 255, 255, 40  })

#define COLOR_PANEL_BG          ((Color){ 30,  30,  50, 255 })
#define COLOR_PANEL_TOP_BORDER  ((Color){ 50,  50,  70, 255 })
#define PANEL_TOP_BORDER_WIDTH  2.0f

// ----- Block Rendering -----
static void DrawSafeTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color c) {
    DrawTriangle(v1, v2, v3, c);
    DrawTriangle(v1, v3, v2, c);
}

static void DrawSafeQuad(Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4, Color c) {
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

        PieceSlot *slot = &state->slots[i];
        if (!slot->piece) continue;

        Piece *p = slot->piece;
        Color clr = PIECE_COLORS[p->colorIndex];

        for (int r = 0; r < p->height; r++) {
            for (int c = 0; c < p->width; c++) {
                if (p->shape[r][c] == 0) continue;
                int x = (int)(slot->posX + c * PANEL_PIECE_SCALE);
                int y = (int)(slot->posY + r * PANEL_PIECE_SCALE);
                DrawBlockBeveled(x, y, PANEL_PIECE_SCALE, clr);

                // Draw gem icon in panel if present
                if (p->gemCells[r][c] != GEM_NONE) {
                    DrawGemIcon(x, y, PANEL_PIECE_SCALE, p->gemCells[r][c]);
                }
            }
        }
    }
}

// ----- Dragged piece (follows mouse at grid scale) -----
void RenderDraggedPiece(GameState *state)
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
            DrawBlockBeveled(x, y, CELL_SIZE, clr);

            // Draw gem icon on dragged piece
            if (p->gemCells[r][c] != GEM_NONE) {
                DrawGemIcon(x, y, CELL_SIZE, p->gemCells[r][c]);
            }
        }
    }
}