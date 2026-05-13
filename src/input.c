#include "input.h"
#include "board.h"
#include "float_text.h"
#include "level.h"
#include "particle.h"
#include "sound.h"

#include <stdio.h>
#include <string.h>

static void TryStartDrag(GameState *state, Vector2 mouse)
{
  if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || state->isDragging)
    return;

  for (int i = 0; i < 3; i++) {
    PieceSlot *slot = &state->slots[i];
    if (!SlotIsOccupied(slot))
      continue;

    float pw = slot->piece.width * PANEL_PIECE_SCALE;
    float ph = slot->piece.height * PANEL_PIECE_SCALE;
    Rectangle rect = {slot->posX, slot->posY, pw, ph};

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

static void AddClearFeedback(GameState *state,
                             bool clearedCells[GRID_SIZE][GRID_SIZE],
                             int savedColors[GRID_SIZE][GRID_SIZE],
                             int points)
{
  AnimAddCleared(&state->anims, clearedCells);

  for (int r = 0; r < GRID_SIZE; r++) {
    for (int c = 0; c < GRID_SIZE; c++) {
      if (!clearedCells[r][c])
        continue;
      float px = GRID_DRAW_X + c * CELL_SIZE + CELL_SIZE / 2.0f;
      float py = GRID_DRAW_Y + r * CELL_SIZE + CELL_SIZE / 2.0f;
      int colorIdx = savedColors[r][c];
      if (colorIdx < 1 || colorIdx > 7)
        colorIdx = 1;
      ParticleEmit(&state->particles, px, py, PIECE_COLORS[colorIdx], 5);
    }
  }

  char scoreBuf[64];
  if (state->combo > 1)
    sprintf(scoreBuf, "+%d (x%d Combo)", points, state->combo);
  else
    sprintf(scoreBuf, "+%d", points);
  FloatTextAdd(&state->floatTexts, scoreBuf, SCREEN_WIDTH / 2.0f,
               SCREEN_HEIGHT / 2.0f - 40.0f, 36,
               (Color){255, 255, 100, 255});

  if (state->combo >= 3) {
    char comboBanner[32];
    if (state->combo >= 5)
      sprintf(comboBanner, "AMAZING! x%d", state->combo);
    else if (state->combo >= 4)
      sprintf(comboBanner, "SUPER COMBO! x%d", state->combo);
    else
      sprintf(comboBanner, "BEST COMBO! x%d", state->combo);
    FloatTextAdd(&state->floatTexts, comboBanner, SCREEN_WIDTH / 2.0f,
                 SCREEN_HEIGHT / 2.0f - 100.0f, 40,
                 (Color){255, 220, 50, 255});
  }
}

static void GenerateNextPiecesIfNeeded(GameState *state)
{
  if (!AllSlotsEmpty(state->slots))
    return;

  const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
  float diamondChance = (def->targetDiamonds > 0) ? DIAMOND_SPAWN_CHANCE : 0.0f;
  float emeraldChance = (def->targetEmeralds > 0) ? EMERALD_SPAWN_CHANCE : 0.0f;
  GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance,
                       emeraldChance);

  for (int i = 0; i < 3; i++) {
    PieceSlot *newSlot = &state->slots[i];
    if (!SlotIsOccupied(newSlot))
      continue;

    float slotCenterX =
        newSlot->posX + (newSlot->piece.width * PANEL_PIECE_SCALE) / 2.0f;
    float slotCenterY =
        newSlot->posY + (newSlot->piece.height * PANEL_PIECE_SCALE) / 2.0f;
    int colorIdx = newSlot->piece.colorIndex;
    if (colorIdx < 1 || colorIdx > 7)
      colorIdx = 1;
    ParticleEmit(&state->particles, slotCenterX, slotCenterY,
                 PIECE_COLORS[colorIdx], 8);
  }
}

static void TryDropDraggedPiece(GameState *state)
{
  if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || !state->isDragging)
    return;

  state->isDragging = false;

  PieceSlot *slot = &state->slots[state->dragSlotIndex];
  if (!SlotIsOccupied(slot))
    return;

  Piece *piece = &slot->piece;
  float pieceScreenX = state->dragPos.x - state->dragOffset.x;
  float pieceScreenY = state->dragPos.y - state->dragOffset.y;

  int gridCol =
      (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE / 2) / CELL_SIZE);
  int gridRow =
      (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE / 2) / CELL_SIZE);

  if (!BoardCanPlace(&state->board, piece, gridRow, gridCol))
    return;

  BoardPlace(&state->board, piece, gridRow, gridCol);
  SoundPlayPlace(&state->sound);

  float placeCenterX =
      GRID_DRAW_X + gridCol * CELL_SIZE + (piece->width * CELL_SIZE) / 2.0f;
  float placeCenterY =
      GRID_DRAW_Y + gridRow * CELL_SIZE + (piece->height * CELL_SIZE) / 2.0f;
  int colorIdx = piece->colorIndex;
  if (colorIdx < 1 || colorIdx > 7)
    colorIdx = 1;
  ParticleEmit(&state->particles, placeCenterX, placeCenterY,
               PIECE_COLORS[colorIdx], 12);

  int savedColors[GRID_SIZE][GRID_SIZE];
  int savedGems[GRID_SIZE][GRID_SIZE];
  memcpy(savedColors, state->board.cells, sizeof(savedColors));
  memcpy(savedGems, state->board.gems, sizeof(savedGems));

  bool clearedCells[GRID_SIZE][GRID_SIZE];
  int linesCleared = BoardClearLines(&state->board, clearedCells);
  int diamondsCollected = 0;
  int emeraldsCollected = 0;

  for (int r = 0; r < GRID_SIZE; r++) {
    for (int c = 0; c < GRID_SIZE; c++) {
      if (clearedCells[r][c] && savedGems[r][c] == GEM_DIAMOND)
        diamondsCollected++;
      else if (clearedCells[r][c] && savedGems[r][c] == GEM_EMERALD)
        emeraldsCollected++;
    }
  }

  if (linesCleared > 0) {
    state->combo += linesCleared;
    int points = ScoreCalculate(linesCleared, state->combo);
    state->score += points;

    SoundPlayLineClear(&state->sound, linesCleared);
    if (state->combo >= 2)
      SoundPlayCombo(&state->sound, state->combo);

    AddClearFeedback(state, clearedCells, savedColors, points);

    if (state->selectedLevel > 0) {
      state->level.collectedDiamonds += diamondsCollected;
      state->level.collectedEmeralds += emeraldsCollected;
    }
  } else {
    state->combo = 0;
  }

  if (state->score > state->highScore)
    state->highScore = state->score;

  SlotClear(slot);
  GenerateNextPiecesIfNeeded(state);
}

void InputUpdate(GameState *state) {
  if (AnimIsActive(&state->anims))
    return;
  if (state->level.levelFailed || state->level.levelComplete)
    return;

  Vector2 mouse = GetMousePosition();

  TryStartDrag(state, mouse);

  if (state->isDragging) {
    state->dragPos = mouse;
  }

  TryDropDraggedPiece(state);
}
