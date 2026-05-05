#ifndef BOARD_H
#define BOARD_H

#include "core/config.h"
#include "core/piece.h"
#include <stdbool.h>

// Board: 8x8 grid where blocks are placed
typedef struct {
    int cells[GRID_SIZE][GRID_SIZE]; // 0 = empty, 1-7 = color index
} Board;

// Initialize all cells to CELL_EMPTY
void BoardInit(Board *board);

// Check if a piece can be placed at grid position (row, col)
bool BoardCanPlace(Board *board, Piece *piece, int row, int col);

// Place piece onto the board at (row, col). Assumes BoardCanPlace was checked.
void BoardPlace(Board *board, Piece *piece, int row, int col);

// Scan and clear full rows/columns. Returns number of lines cleared.
int BoardClearLines(Board *board, bool clearedCells[GRID_SIZE][GRID_SIZE]);

// Check if any remaining piece in slots can fit anywhere on the board
bool BoardHasValidMove(Board *board, PieceSlot slots[3]);

#endif // BOARD_H