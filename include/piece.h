#ifndef PIECE_H
#define PIECE_H

#include "defs.h"
#include <stdbool.h>

// A block piece with its shape matrix and metadata
typedef struct {
    int shape[MAX_PIECE_SIZE][MAX_PIECE_SIZE]; // 1 = filled, 0 = empty
    int width;
    int height;
    int colorIndex; // 1-7 maps to a color in the palette
    int gemCells[MAX_PIECE_SIZE][MAX_PIECE_SIZE]; // GEM_NONE, GEM_DIAMOND, GEM_EMERALD
} Piece;

// A slot in the bottom panel holding one piece.
// occupied=false means the player already used this slot.
typedef struct {
    Piece piece;
    bool occupied;
    float posX;      // screen X for drawing in bottom panel
    float posY;      // screen Y for drawing in bottom panel
} PieceSlot;

// Static piece shape definition (used as templates)
typedef struct {
    int shape[MAX_PIECE_SIZE][MAX_PIECE_SIZE];
    int width;
    int height;
} PieceDef;

// Get the array of all piece definitions
const PieceDef *GetPieceDefinitions(void);

// Fill out with a random piece definition and color.
// Pass 0,0 for no gems (classic mode).
void PieceGenerate(Piece *out, float diamondChance, float emeraldChance);

void SlotClear(PieceSlot *slot);
bool SlotIsOccupied(const PieceSlot *slot);

// Generate 3 random pieces into the given slots
// Pass 0,0 for gem chances in classic mode
void GenerateRandomPieces(PieceSlot slots[3], float panelY, float screenWidth,
                           float diamondChance, float emeraldChance);

// Check if all 3 slots are empty (pieces used up)
bool AllSlotsEmpty(PieceSlot slots[3]);

#endif // PIECE_H
