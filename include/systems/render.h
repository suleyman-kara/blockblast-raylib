#ifndef RENDER_H
#define RENDER_H

#include "game.h"

// Draw the entire game frame based on current state
void RenderFrame(GameState *state);

// Sub-render functions (called by RenderFrame or directly)
void RenderBoard(GameState *state);
void RenderGhost(GameState *state);
void RenderPieceSlots(GameState *state);
void RenderDraggedPiece(GameState *state);
void RenderScore(GameState *state);
void RenderBanner(GameState *state);
void RenderGearIcon(void);
void RenderMenu(GameState *state);
void RenderSettings(GameState *state);
void RenderGameOver(GameState *state);
void RenderAdventureMap(GameState *state);
void RenderAdventurePlay(GameState *state);
void RenderAdventureResult(GameState *state);

#endif // RENDER_H