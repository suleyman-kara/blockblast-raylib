#include "input.h"
#include "sound.h"
#include "board.h"
#include "score.h"
#include "float_text.h"
#include "particle.h"
#include "adventure.h"

#include <stdio.h>
#include <string.h>

void InputUpdate(GameState *state)
{
    if (AnimIsActive(&state->anims)) return;
    if (state->gameOver) return;

    Vector2 mouse = GetMousePosition();

    // --- DRAG START ---
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !state->isDragging) {
        for (int i = 0; i < 3; i++) {
            PieceSlot *slot = &state->slots[i];
            if (!slot->piece) continue;

            float pw = slot->piece->width * PANEL_PIECE_SCALE;
            float ph = slot->piece->height * PANEL_PIECE_SCALE;
            Rectangle rect = { slot->posX, slot->posY, pw, ph };

            if (CheckCollisionPointRec(mouse, rect)) {
                state->isDragging = true;
                state->dragSlotIndex = i;
                state->dragOffset.x = mouse.x - slot->posX;
                state->dragOffset.y = mouse.y - slot->posY;
                state->dragPos = mouse;
                break;
            }
        }
    }

    // --- DRAG MOVE ---
    if (state->isDragging) {
        state->dragPos = mouse;
    }

    // --- DRAG END (drop) ---
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state->isDragging) {
        state->isDragging = false;

        PieceSlot *slot = &state->slots[state->dragSlotIndex];
        Piece *piece = slot->piece;
        if (!piece) return;

        float pieceScreenX = state->dragPos.x - state->dragOffset.x;
        float pieceScreenY = state->dragPos.y - state->dragOffset.y;

        // Snap to nearest grid cell
        int gridCol = (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE / 2) / CELL_SIZE);
        int gridRow = (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE / 2) / CELL_SIZE);

        if (BoardCanPlace(&state->board, piece, gridRow, gridCol)) {
            BoardPlace(&state->board, piece, gridRow, gridCol);
            SoundPlayPlace(&state->sound);

            // Save cell colors BEFORE clearing (needed for particle colors)
            int savedColors[GRID_SIZE][GRID_SIZE];
            memcpy(savedColors, state->board.cells, sizeof(savedColors));

            bool clearedCells[GRID_SIZE][GRID_SIZE];
            int linesCleared = BoardClearLines(&state->board, clearedCells);

            if (linesCleared > 0) {
                state->combo++;
                int points = ScoreCalculate(linesCleared, state->combo);
                state->score += points;

                // Play line clear sound
                SoundPlayLineClear(&state->sound, linesCleared);

                // Play combo sound (combo >= 2)
                if (state->combo >= 2)
                    SoundPlayCombo(&state->sound, state->combo);

                // Add clear animation
                AnimAddCleared(&state->anims, clearedCells);

                // Emit particles from cleared cells
                for (int r = 0; r < GRID_SIZE; r++) {
                    for (int c = 0; c < GRID_SIZE; c++) {
                        if (!clearedCells[r][c]) continue;
                        float px = GRID_DRAW_X + c * CELL_SIZE + CELL_SIZE / 2.0f;
                        float py = GRID_DRAW_Y + r * CELL_SIZE + CELL_SIZE / 2.0f;
                        int colorIdx = savedColors[r][c];
                        if (colorIdx < 1 || colorIdx > 7) colorIdx = 1;
                        ParticleEmit(&state->particles, px, py,
                                     PIECE_COLORS[colorIdx], 5);
                    }
                }

                // Floating score text
                char scoreBuf[32];
                sprintf(scoreBuf, "+%d", points);
                FloatTextAdd(&state->floatTexts, scoreBuf,
                             SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 40.0f,
                             36, (Color){255, 255, 100, 255});

                // Best Combo banner (combo >= 3)
                if (state->combo >= 3) {
                    state->banner.active = true;
                    state->banner.timer = BANNER_DURATION;
                    state->banner.duration = BANNER_DURATION;
                    if (state->combo >= 5)
                        sprintf(state->banner.text, "MUHTESEM! x%d", state->combo);
                    else if (state->combo >= 4)
                        sprintf(state->banner.text, "SUPER COMBO! x%d", state->combo);
                    else
                        sprintf(state->banner.text, "BEST COMBO! x%d", state->combo);
                }

                // Update adventure progress if in adventure mode
                if (state->currentScreen == SCREEN_ADVENTURE_PLAY) {
                    AdventureUpdateProgress(&state->adventure, &state->board,
                                            linesCleared, state->combo, points);
                }
            } else {
                state->combo = 0;
            }

            if (state->score > state->highScore)
                state->highScore = state->score;

            PieceFree(&slot->piece);

            // Track pieces used in adventure mode
            if (state->currentScreen == SCREEN_ADVENTURE_PLAY) {
                state->adventure.piecesUsed++;
            }

            if (AllSlotsEmpty(state->slots))
                GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);

            if (!BoardHasValidMove(&state->board, state->slots)) {
                state->gameOver = true;
                ScoreSaveHigh(state->highScore);
            }
        }
    }
}