#include "render.h"
#include "particle.h"
#include "float_text.h"

void RenderFrame(GameState *state)
{
    BeginDrawing();
    ClearBackground(BG_COLOR);

    switch (state->currentScreen) {
        case SCREEN_MENU:
            RenderMenu(state);
            break;
        case SCREEN_PLAY:
            RenderScore(state);
            RenderBoard(state);
            RenderGhost(state);
            RenderPieceSlots(state);
            RenderDraggedPiece(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            RenderBanner(state);
            RenderGearIcon();
            break;
        case SCREEN_SETTINGS:
            // Draw the game behind the overlay
            if (state->prevScreen == SCREEN_ADVENTURE_PLAY) {
                RenderAdventurePlay(state);
            } else {
                RenderScore(state);
                RenderBoard(state);
                RenderPieceSlots(state);
                ParticleDraw(&state->particles);
                FloatTextDraw(&state->floatTexts);
                RenderBanner(state);
            }
            // Settings overlay on top
            RenderSettings(state);
            break;
        case SCREEN_GAMEOVER:
            RenderScore(state);
            RenderBoard(state);
            ParticleDraw(&state->particles);
            RenderGameOver(state);
            break;
        case SCREEN_ADVENTURE_MAP:
            RenderAdventureMap(state);
            break;
        case SCREEN_ADVENTURE_PLAY:
            RenderAdventurePlay(state);
            RenderGhost(state);
            RenderDraggedPiece(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            RenderBanner(state);
            RenderGearIcon();
            break;
        case SCREEN_ADVENTURE_RESULT:
            RenderAdventureResult(state);
            break;
    }

    EndDrawing();
}