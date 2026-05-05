#include "piece.h"
#include <stdlib.h>
#include <time.h>

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

static bool seeded = false;

const PieceDef *GetPieceDefinitions(void)
{
    return PIECE_DEFS;
}

Piece *PieceCreate(void)
{
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    Piece *p = (Piece *)malloc(sizeof(Piece));
    if (!p) return NULL;

    int defIndex = rand() % PIECE_DEF_COUNT;
    const PieceDef *def = &PIECE_DEFS[defIndex];

    // Copy shape
    for (int r = 0; r < MAX_PIECE_SIZE; r++)
        for (int c = 0; c < MAX_PIECE_SIZE; c++)
            p->shape[r][c] = def->shape[r][c];

    p->width = def->width;
    p->height = def->height;
    p->colorIndex = (rand() % 7) + 1; // 1-7

    return p;
}

void PieceFree(Piece **piece)
{
    if (piece && *piece) {
        free(*piece);
        *piece = NULL;
    }
}

void GenerateRandomPieces(PieceSlot slots[3], float panelY, float screenWidth)
{
    // Divide the bottom panel into 3 equal sections
    float sectionWidth = screenWidth / 3.0f;

    for (int i = 0; i < 3; i++) {
        slots[i].piece = PieceCreate();
        // Center each piece in its section
        if (slots[i].piece) {
            float piecePixelW = slots[i].piece->width * 25.0f;  // smaller scale in panel
            slots[i].posX = sectionWidth * i + (sectionWidth - piecePixelW) / 2.0f;
            slots[i].posY = panelY + 20.0f;
        }
    }
}

bool AllSlotsEmpty(PieceSlot slots[3])
{
    for (int i = 0; i < 3; i++) {
        if (slots[i].piece != NULL) return false;
    }
    return true;
}