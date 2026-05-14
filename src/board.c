#include "board.h"
#include <string.h>
#include <stdlib.h>

void BoardInit(Board *board)
{
    memset(board->cells, CELL_EMPTY, sizeof(board->cells));
    memset(board->gems, GEM_NONE, sizeof(board->gems));
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
            board->gems[row + r][col + c] = piece->gemCells[r][c];
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

    // Check which rows are full
    for (int r = 0; r < GRID_SIZE; r++) {
        rowFull[r] = true;
        for (int c = 0; c < GRID_SIZE; c++) {
            if (board->cells[r][c] == CELL_EMPTY) {
                rowFull[r] = false;
                break;
            }
        }
        if (rowFull[r]) cleared++;
    }

    // Check which columns are full
    for (int c = 0; c < GRID_SIZE; c++) {
        colFull[c] = true;
        for (int r = 0; r < GRID_SIZE; r++) {
            if (board->cells[r][c] == CELL_EMPTY) {
                colFull[c] = false;
                break;
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
            board->gems[r][c] = GEM_NONE;
        }
    }

    // Mark and clear full columns
    for (int c = 0; c < GRID_SIZE; c++) {
        if (!colFull[c]) continue;
        for (int r = 0; r < GRID_SIZE; r++) {
            clearedCells[r][c] = true;
            board->cells[r][c] = CELL_EMPTY;
            board->gems[r][c] = GEM_NONE;
        }
    }

    return cleared;
}


bool BoardHasValidMove(Board *board, PieceSlot slots[3])
{
    for (int s = 0; s < 3; s++) {
        if (!SlotIsOccupied(&slots[s])) continue;

        Piece *p = &slots[s].piece;
        for (int r = 0; r <= GRID_SIZE - p->height; r++) {
            for (int c = 0; c <= GRID_SIZE - p->width; c++) {
                if (BoardCanPlace(board, p, r, c))
                    return true;
            }
        }
    }
    return false;
}

void BoardPrefillGems(Board *board, int count, bool allowDiamonds,
                      bool allowEmeralds)
{
    // Use the piece definitions to get random shapes
    const PieceDef *defs = GetPieceDefinitions();

    for (int i = 0; i < count; i++) {
        // Pick a random piece definition
        int defIndex = rand() % PIECE_DEF_COUNT;
        const PieceDef *def = &defs[defIndex];

        // Try to find a valid position (max 50 attempts)
        for (int attempt = 0; attempt < 50; attempt++) {
            int row = rand() % (GRID_SIZE - def->height + 1);
            int col = rand() % (GRID_SIZE - def->width + 1);

            // Use BoardCanPlace logic via a temporary Piece
            Piece tempPiece;
            memset(&tempPiece, 0, sizeof(tempPiece));
            memcpy(tempPiece.shape, def->shape, sizeof(def->shape));
            tempPiece.width = def->width;
            tempPiece.height = def->height;
            tempPiece.colorIndex = 1;

            if (!BoardCanPlace(board, &tempPiece, row, col)) continue;

            // Place the piece with a random color
            int colorIdx = (rand() % 7) + 1;
            for (int r = 0; r < def->height; r++) {
                for (int c = 0; c < def->width; c++) {
                    if (def->shape[r][c] == 0) continue;
                    board->cells[row + r][col + c] = colorIdx;

                    // Assign only target-relevant gems to some cells.
                    if ((allowDiamonds || allowEmeralds) && (rand() % 100) < 30) {
                        if (allowDiamonds && allowEmeralds) {
                            board->gems[row + r][col + c] =
                                (rand() % 2) == 0 ? GEM_DIAMOND : GEM_EMERALD;
                        } else if (allowDiamonds) {
                            board->gems[row + r][col + c] = GEM_DIAMOND;
                        } else {
                            board->gems[row + r][col + c] = GEM_EMERALD;
                        }
                    }
                }
            }
            break; // Successfully placed
        }
    }
}
