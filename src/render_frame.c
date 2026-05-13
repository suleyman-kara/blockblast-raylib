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
            RenderPlayHUD(state);
            RenderBoard(state);
            RenderGhost(state);
            RenderPieceSlots(state);
            RenderDraggedPiece(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            RenderGearIcon();
            break;
        case SCREEN_SETTINGS:
            // Draw the game behind the overlay
            RenderPlayHUD(state);
            RenderBoard(state);
            RenderPieceSlots(state);
            ParticleDraw(&state->particles);
            FloatTextDraw(&state->floatTexts);
            // Settings overlay on top
            RenderSettings(state);
            break;
        case SCREEN_LEVEL_SELECT:
            RenderLevelSelect(state);
            break;
        case SCREEN_RESULT:
            RenderResult(state);
            break;
        case SCREEN_NICKNAME:
            RenderNickname(state);
            break;
        case SCREEN_SCOREBOARD:
            RenderScoreboard(state);
            break;
        case SCREEN_MENU_SETTINGS:
            RenderMenuSettings(state);
            break;
    }

    EndDrawing();
}
