#include "render.h"
#include "adventure.h"

// ----- Board -----
void RenderBoard(GameState *state)
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
            } else if (val == CELL_ICE) {
                // Ice block: translucent blue with color underneath
                int underColor = AdventureGetIceUnderColor(&state->adventure, r, c);
                if (underColor >= 1 && underColor <= 7) {
                    DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, PIECE_COLORS[underColor]);
                }
                // Draw ice overlay
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, ICE_COLOR);
                // Ice crystal effect (small white diamond)
                DrawRectangle(x + 15, y + 15, CELL_SIZE - 30, CELL_SIZE - 30, (Color){255,255,255,60});
                DrawRectangle(x + 20, y + 20, CELL_SIZE - 40, CELL_SIZE - 40, (Color){255,255,255,40});
            } else if (val == CELL_STONE) {
                // Stone block: gray with rock texture
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, STONE_COLOR);
                DrawRectangle(x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6, (Color){90, 90, 100, 255});
                DrawRectangle(x + 8, y + 8, CELL_SIZE - 16, CELL_SIZE - 16, (Color){70, 70, 80, 255});
                // Small highlight
                DrawRectangle(x + 10, y + 10, 5, 5, (Color){120, 120, 130, 100});
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
            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, ghost);
        }
    }
}

// ----- Piece slots (bottom panel) -----
void RenderPieceSlots(GameState *state)
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
            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
        }
    }
}