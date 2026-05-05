#ifndef PIECE_H
#define PIECE_H

#include "core/config.h"
#include <stdbool.h>

// A block piece with its shape matrix and metadata
typedef struct {
    int shape[MAX_PIECE_SIZE][MAX_PIECE_SIZE]; // 1 = filled, 0 = empty
    int width;
    int height;
    int colorIndex; // 1-7 maps to a color in the palette
} Piece;

// A slot in the bottom panel holding one piece (or NULL if used)
typedef struct {
    Piece *piece;    // dynamically allocated, NULL = already used
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

// Create a piece (malloc) from a random definition with a random color
Piece *PieceCreate(void);

// Free a piece's memory and set the pointer to NULL
void PieceFree(Piece **piece);

// Generate 3 random pieces into the given slots
void GenerateRandomPieces(PieceSlot slots[3], float panelY, float screenWidth);

// Check if all 3 slots are empty (pieces used up)
bool AllSlotsEmpty(PieceSlot slots[3]);

#endif // PIECE_H