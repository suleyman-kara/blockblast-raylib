#include "board.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

void BoardCollectGems(Board *board, bool clearedCells[GRID_SIZE][GRID_SIZE],
                      int *diamondsCollected, int *emeraldsCollected)
{
    if (diamondsCollected) *diamondsCollected = 0;
    if (emeraldsCollected) *emeraldsCollected = 0;

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (!clearedCells[r][c]) continue;
            if (board->gems[r][c] == GEM_DIAMOND && diamondsCollected)
                (*diamondsCollected)++;
            else if (board->gems[r][c] == GEM_EMERALD && emeraldsCollected)
                (*emeraldsCollected)++;
        }
    }
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

void BoardPrefillGems(Board *board, int count, int levelIndex)
{
    // Seed random if not already seeded
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

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
                    // Assign a gem to some cells (roughly 30% chance)
                    if ((rand() % 100) < 30) {
                        // Diamond for even levels, mix for odd
                        if (levelIndex < 4 || (rand() % 2) == 0) {
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