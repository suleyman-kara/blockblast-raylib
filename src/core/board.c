#include "board.h"
#include <string.h>

void BoardInit(Board *board)
{
    memset(board->cells, CELL_EMPTY, sizeof(board->cells));
}

bool BoardCanPlace(Board *board, Piece *piece, int row, int col)
{
    if (!piece) return false;

    for (int r = 0; r < piece->height; r++) {
        for (int c = 0; c < piece->width; c++) {
            if (piece->shape[r][c] == 0) continue;

            int gr = row + r;
            int gc = col + c;

            // Out of bounds check
            if (gr < 0 || gr >= GRID_SIZE || gc < 0 || gc >= GRID_SIZE)
                return false;

            // Collision check (empty, ice, or stone all block placement)
            if (board->cells[gr][gc] != CELL_EMPTY)
                return false;
        }
    }
    return true;
}

void BoardPlace(Board *board, Piece *piece, int row, int col)
{
    for (int r = 0; r < piece->height; r++) {
        for (int c = 0; c < piece->width; c++) {
            if (piece->shape[r][c] == 0) continue;
            board->cells[row + r][col + c] = piece->colorIndex;
        }
    }
}

int BoardClearLines(Board *board, bool clearedCells[GRID_SIZE][GRID_SIZE])
{
    int cleared = 0;
    bool rowFull[GRID_SIZE] = {false};
    bool colFull[GRID_SIZE] = {false};

    // Initialize clearedCells to false
    memset(clearedCells, false, sizeof(bool) * GRID_SIZE * GRID_SIZE);

    // Check which rows are full (skip rows with stone blocks)
    for (int r = 0; r < GRID_SIZE; r++) {
        rowFull[r] = true;
        for (int c = 0; c < GRID_SIZE; c++) {
            if (board->cells[r][c] == CELL_STONE) {
                rowFull[r] = false;
                break;
            }
            if (board->cells[r][c] == CELL_EMPTY || board->cells[r][c] == CELL_ICE) {
                rowFull[r] = false;
            }
        }
        if (rowFull[r]) cleared++;
    }

    // Check which columns are full (skip columns with stone blocks)
    for (int c = 0; c < GRID_SIZE; c++) {
        colFull[c] = true;
        for (int r = 0; r < GRID_SIZE; r++) {
            if (board->cells[r][c] == CELL_STONE) {
                colFull[c] = false;
                break;
            }
            if (board->cells[r][c] == CELL_EMPTY || board->cells[r][c] == CELL_ICE) {
                colFull[c] = false;
            }
        }
        if (colFull[c]) cleared++;
    }

    // Mark and clear full rows
    for (int r = 0; r < GRID_SIZE; r++) {
        if (!rowFull[r]) continue;
        for (int c = 0; c < GRID_SIZE; c++) {
            clearedCells[r][c] = true;
            board->cells[r][c] = CELL_EMPTY;
        }
    }

    // Mark and clear full columns
    for (int c = 0; c < GRID_SIZE; c++) {
        if (!colFull[c]) continue;
        for (int r = 0; r < GRID_SIZE; r++) {
            clearedCells[r][c] = true;
            board->cells[r][c] = CELL_EMPTY;
        }
    }

    return cleared;
}

bool BoardHasValidMove(Board *board, PieceSlot slots[3])
{
    for (int s = 0; s < 3; s++) {
        if (slots[s].piece == NULL) continue;

        Piece *p = slots[s].piece;
        for (int r = 0; r <= GRID_SIZE - p->height; r++) {
            for (int c = 0; c <= GRID_SIZE - p->width; c++) {
                if (BoardCanPlace(board, p, r, c))
                    return true;
            }
        }
    }
    return false;
}