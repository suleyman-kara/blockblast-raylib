#ifndef RENDER_H
#define RENDER_H

#include "game.h"

// Draw the entire game frame based on current state
void RenderFrame(GameState *state);

// Sub-render functions (called by RenderFrame)
void RenderBoard(GameState *state);
void RenderGhost(GameState *state);
void RenderPieceSlots(GameState *state);
void RenderDraggedPiece(GameState *state);
void RenderPlayHUD(GameState *state);
void RenderGearIcon(void);
void DrawGemIcon(int x, int y, int cellSize, int gemType);
void RenderMenu(GameState *state);
void RenderSettings(GameState *state);
void RenderLevelSelect(GameState *state);
void RenderResult(GameState *state);

#endif // RENDER_H