#include "piece.h"
#include <string.h>
#include <stdlib.h>

// ---- All piece shape definitions ----
static const PieceDef PIECE_DEFS[PIECE_DEF_COUNT] = {
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
