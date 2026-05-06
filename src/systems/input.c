#include "input.h"
#include "sound.h"
#include "board.h"
#include "score.h"
#include "float_text.h"
#include "particle.h"
#include "adventure.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

void InputUpdate(GameState *state)
{
    if (AnimIsActive(&state->anims)) return;
    if (state->gameOver) return;

    Vector2 mouse = GetMousePosition();

    // --- DRAG START ---
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !state->isDragging) {
        // First check if clicking directly on a piece (original behavior)
        bool clickedOnPiece = false;
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
                state->dragStartPos = mouse;
                clickedOnPiece = true;
                break;
            }
        }

        // If not clicked on a piece, check bottom zone (divided by 3)
        if (!clickedOnPiece && mouse.y >= BOTTOM_ZONE_Y) {
            float zoneWidth = (float)SCREEN_WIDTH / 3.0f;
            int slotIndex = (int)(mouse.x / zoneWidth);
            if (slotIndex < 0) slotIndex = 0;
            if (slotIndex > 2) slotIndex = 2;

            PieceSlot *slot = &state->slots[slotIndex];
            if (slot->piece) {
                state->isDragging = true;
                state->dragSlotIndex = slotIndex;
                // Center the drag offset on the piece
                float pw = slot->piece->width * PANEL_PIECE_SCALE;
                float ph = slot->piece->height * PANEL_PIECE_SCALE;
                state->dragOffset.x = pw / 2.0f;
                state->dragOffset.y = ph / 2.0f;
                state->dragPos = mouse;
                state->dragStartPos = mouse;
            }
        }
    }

    // --- DRAG MOVE (with speed increase based on distance from origin) ---
    if (state->isDragging) {
        // Calculate distance from the initial drag start position
        float dx = mouse.x - state->dragStartPos.x;
        float dy = mouse.y - state->dragStartPos.y;
        float distance = sqrtf(dx * dx + dy * dy);

        // The piece position is determined by applying a speed multiplier
        // based on distance from the original click point.
        // speed = base + distance * factor
        // This means the piece follows the mouse but with an extra offset
        // that grows as it moves further from its origin.
        float speedBoost = 1.0f + distance * 0.005f; // half the original acceleration
        if (speedBoost < 1.0f) speedBoost = 1.0f;

        // Apply the boosted position relative to dragStartPos
        state->dragPos.x = state->dragStartPos.x + dx * speedBoost;
        state->dragPos.y = state->dragStartPos.y + dy * speedBoost;
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

            // Emit particles at the placement position
            float placeCenterX = GRID_DRAW_X + gridCol * CELL_SIZE + (piece->width * CELL_SIZE) / 2.0f;
            float placeCenterY = GRID_DRAW_Y + gridRow * CELL_SIZE + (piece->height * CELL_SIZE) / 2.0f;
            int colorIdx = piece->colorIndex;
            if (colorIdx < 1 || colorIdx > 7) colorIdx = 1;
            ParticleEmit(&state->particles, placeCenterX, placeCenterY,
                         PIECE_COLORS[colorIdx], 12);

            // Save cell colors BEFORE clearing (needed for particle colors)
            int savedColors[GRID_SIZE][GRID_SIZE];
            memcpy(savedColors, state->board.cells, sizeof(savedColors));

            bool clearedCells[GRID_SIZE][GRID_SIZE];
            int diamondsCollected = 0;
            int emeraldsCollected = 0;
            int linesCleared = BoardClearLines(&state->board, clearedCells,
                                                &diamondsCollected, &emeraldsCollected);

            if (linesCleared > 0) {
                state->combo += linesCleared;
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
                        int colorIdx2 = savedColors[r][c];
                        if (colorIdx2 < 1 || colorIdx2 > 7) colorIdx2 = 1;
                        ParticleEmit(&state->particles, px, py,
                                     PIECE_COLORS[colorIdx2], 5);
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
                                            linesCleared, state->combo, points,
                                            diamondsCollected, emeraldsCollected);
                }
            } else {
                state->combo = 0;
            }

            if (state->score > state->highScore)
                state->highScore = state->score;

            PieceFree(&slot->piece);

            if (AllSlotsEmpty(state->slots)) {
                // Generate new pieces (with gems if in adventure mode)
                if (state->currentScreen == SCREEN_ADVENTURE_PLAY) {
                    const LevelDef *level = AdventureGetLevelDefs();
                    level = &level[state->adventure.currentLevel];
                    float diamondChance = 0.0f;
                    float emeraldChance = 0.0f;
                    if (level->goalType == GOAL_GEMS || level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
                        diamondChance = DIAMOND_SPAWN_CHANCE;
                        if (level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
                            emeraldChance = EMERALD_SPAWN_CHANCE;
                        }
                    }
                    GenerateRandomPiecesWithGems(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance, emeraldChance);
                } else {
                    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);
                }

                // Emit particles for new block generation
                for (int i = 0; i < 3; i++) {
                    PieceSlot *newSlot = &state->slots[i];
                    if (newSlot->piece) {
                        float slotCenterX = newSlot->posX + (newSlot->piece->width * PANEL_PIECE_SCALE) / 2.0f;
                        float slotCenterY = newSlot->posY + (newSlot->piece->height * PANEL_PIECE_SCALE) / 2.0f;
                        int newColorIdx = newSlot->piece->colorIndex;
                        if (newColorIdx < 1 || newColorIdx > 7) newColorIdx = 1;
                        ParticleEmit(&state->particles, slotCenterX, slotCenterY,
                                     PIECE_COLORS[newColorIdx], 8);
                    }
                }
            }

            if (!BoardHasValidMove(&state->board, state->slots)) {
                state->gameOver = true;
                ScoreSaveHigh(state->highScore);
            }
        }
    }
}