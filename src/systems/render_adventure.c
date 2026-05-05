#include "render.h"
#include "font.h"
#include "adventure.h"
#include "theme.h"
#include <stdio.h>

// ----- Adventure Map -----
void RenderAdventureMap(GameState *state)
{
    const AdventureMapStyle *m = &THEME_DEFAULT.adventureMap;
    const TextStyle *txt = &THEME_DEFAULT.text;

    // Title
    const char *mapTitle = "MACERA MODU";
    GameDrawText(mapTitle, (SCREEN_WIDTH - GameMeasureText(mapTitle, 36)) / 2, 30, 36, txt->primary);

    // Level grid
    int totalRowWidth = LEVELS_PER_ROW * m->btnSize + (LEVELS_PER_ROW - 1) * m->btnGap;
    int mapStartX = (SCREEN_WIDTH - totalRowWidth) / 2;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        int row = i / LEVELS_PER_ROW;
        int col = i % LEVELS_PER_ROW;
        int bx = mapStartX + col * (m->btnSize + m->btnGap);
        int by = m->mapStartY + row * (m->btnSize + m->btnGap + m->btnLabelGap);
        Rectangle btnRect = { bx, by, m->btnSize, m->btnSize };

        bool isUnlocked = state->adventureSave.levels[i].unlocked;
        bool isCompleted = state->adventureSave.levels[i].completed;
        bool hover = CheckCollisionPointRec(mouse, btnRect) && isUnlocked;

        // Button background
        Color btnColor;
        Color borderColor;
        if (!isUnlocked) {
            btnColor = m->lockedBg;
            borderColor = m->lockedBorder;
        } else if (hover) {
            btnColor = m->unlockedBgHover;
            borderColor = m->unlockedBorder;
        } else {
            btnColor = m->unlockedBg;
            borderColor = m->unlockedBorder;
        }

        DrawRectangleRounded(btnRect, 0.2f, 6, btnColor);
        DrawRectangleRoundedLines(btnRect, 0.2f, 6, borderColor);

        // Level number
        char lvlBuf[8];
        sprintf(lvlBuf, "%d", i + 1);
        int lnw = GameMeasureText(lvlBuf, 28);
        Color lvlColor = isUnlocked ? txt->primary : m->lockedNumber;
        GameDrawText(lvlBuf, bx + (m->btnSize - lnw) / 2, by + 20, 28, lvlColor);

        // Status indicator
        if (isCompleted) {
            // Green checkmark
            GameDrawText("✓", bx + m->btnSize - 22, by + 5, 18, m->completedText);
        } else if (!isUnlocked) {
            // Lock icon
            GameDrawText("K", bx + m->btnSize / 2 - 8, by + m->btnSize - 22, 18, m->lockedNumber);
        }
    }

    // Hint at bottom
    const char *escHint = "ESC: Ana Menu";
    GameDrawText(escHint, (SCREEN_WIDTH - GameMeasureText(escHint, 16)) / 2,
             SCREEN_HEIGHT - 30, 16, txt->muted);
}

// ----- Adventure Play HUD -----
void RenderAdventurePlay(GameState *state)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[state->adventure.currentLevel];
    const TextStyle *txt = &THEME_DEFAULT.text;

    char buf[128];

    // Compact top bar: Level (left) | Goal (right)
    int topY = 8;
    sprintf(buf, "Level %d", state->adventure.currentLevel + 1);
    GameDrawText(buf, 10, topY, 20, (Color){180, 150, 255, 255});

    // Goal description (right side)
    sprintf(buf, "Hedef: %s", level->description);
    int gw = GameMeasureText(buf, 14);
    GameDrawText(buf, SCREEN_WIDTH - gw - 10, topY + 2, 14, txt->secondary);

    // Progress bar (compact, above board)
    int barY = 35;
    int barWidth = SCREEN_WIDTH - 60;
    int barHeight = 12;
    int barX = 30;

    // Calculate overall progress
    float progress = 0.0f;
    int metGoals = 0;

    switch (level->goalType) {
        case GOAL_SCORE:
            if (state->adventure.goalScore > 0) {
                progress = (float)state->score / (float)state->adventure.goalScore;
            }
            break;

        case GOAL_GEMS:
            if (state->adventure.goalDiamonds > 0) {
                progress = (float)state->adventure.collectedDiamonds / (float)state->adventure.goalDiamonds;
            }
            break;

        case GOAL_MIXED_GEMS:
            if (state->adventure.goalDiamonds > 0) metGoals += state->adventure.collectedDiamonds;
            if (state->adventure.goalEmeralds > 0) metGoals += state->adventure.collectedEmeralds;
            progress = (float)metGoals / (float)(state->adventure.goalDiamonds + state->adventure.goalEmeralds);
            break;

        case GOAL_MIXED_ALL: {
            float p1 = (float)state->score / (float)state->adventure.goalScore;
            float p2 = (float)state->adventure.collectedDiamonds / (float)state->adventure.goalDiamonds;
            float p3 = (float)state->adventure.collectedEmeralds / (float)state->adventure.goalEmeralds;
            progress = (p1 + p2 + p3) / 3.0f;
            break;
        }
    }
    if (progress > 1.0f) progress = 1.0f;

    // Bar background
    DrawRectangle(barX, barY, barWidth, barHeight, (Color){40, 40, 60, 255});
    DrawRectangleLines(barX, barY, barWidth, barHeight, (Color){60, 60, 80, 255});

    // Bar fill
    Color barColor;
    if (progress < 0.5f)
        barColor = (Color){255, 100, 100, 255};
    else if (progress < 0.8f)
        barColor = (Color){255, 200, 50, 255};
    else
        barColor = (Color){50, 255, 100, 255};

    DrawRectangle(barX + 1, barY + 1,
                  (int)((barWidth - 2) * progress), barHeight - 2, barColor);

    // Progress text inside bar
    switch (level->goalType) {
        case GOAL_SCORE:
            sprintf(buf, "%d / %d", state->score, state->adventure.goalScore);
            break;
        case GOAL_GEMS:
            sprintf(buf, "◆ %d / %d", state->adventure.collectedDiamonds, state->adventure.goalDiamonds);
            break;
        case GOAL_MIXED_GEMS:
            sprintf(buf, "◆ %d/%d  ◆ %d/%d",
                    state->adventure.collectedDiamonds, state->adventure.goalDiamonds,
                    state->adventure.collectedEmeralds, state->adventure.goalEmeralds);
            break;
        case GOAL_MIXED_ALL:
            sprintf(buf, "%dp ◆%d ◆%d", state->score,
                    state->adventure.collectedDiamonds, state->adventure.collectedEmeralds);
            break;
    }
    int pw = GameMeasureText(buf, 12);
    GameDrawText(buf, (SCREEN_WIDTH - pw) / 2, barY + 1, 12, (Color){220, 220, 240, 255});

    // Info line: score (compact, below bar)
    int infoY = barY + barHeight + 4;
    if (level->goalType == GOAL_SCORE || level->goalType == GOAL_MIXED_ALL) {
        sprintf(buf, "Puan: %d", state->score);
        int sw = GameMeasureText(buf, 14);
        GameDrawText(buf, SCREEN_WIDTH - sw - 10, infoY, 14, txt->secondary);
    }

    // Render board
    RenderBoard(state);

    // Render piece slots
    RenderPieceSlots(state);
}

// ----- Adventure Result Screen -----
void RenderAdventureResult(GameState *state)
{
    const LevelDef *level = AdventureGetLevelDefs();
    level = &level[state->adventure.currentLevel];

    // Draw the game board underneath (semi-transparent overlay on top)
    RenderAdventurePlay(state);

    // Semi-transparent dark overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    // ─── Card background ──────────────────────────────────────────────────────
    int cardW = 380, cardH = 400;
    int cardX = (SCREEN_WIDTH - cardW) / 2;
    int cardY = (SCREEN_HEIGHT - cardH) / 2 - 20;
    DrawRectangleRounded((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){20, 25, 50, 235});
    DrawRectangleRoundedLines((Rectangle){cardX, cardY, cardW, cardH}, 0.15f, 8, (Color){60, 70, 110, 180});

    char buf[128];
    Vector2 mouse = GetMousePosition();

    // ─── Title ────────────────────────────────────────────────────────────────
    if (state->adventure.levelFailed) {
        const char *failMsg = "SEVIYE BASARISIZ!";
        int fw = GameMeasureText(failMsg, 32);
        GameDrawText(failMsg, (SCREEN_WIDTH - fw) / 2, cardY + 30, 32, (Color){255, 80, 80, 255});
    } else {
        const char *successMsg = "SEVIYE TAMAMLANDI!";
        int sw = GameMeasureText(successMsg, 32);
        GameDrawText(successMsg, (SCREEN_WIDTH - sw) / 2, cardY + 30, 32, (Color){50, 255, 100, 255});
    }

    // ─── Level info ───────────────────────────────────────────────────────────
    sprintf(buf, "Level %d", state->adventure.currentLevel + 1);
    int lw = GameMeasureText(buf, 20);
    GameDrawText(buf, (SCREEN_WIDTH - lw) / 2, cardY + 75, 20, (Color){180, 150, 255, 255});

    // ─── Stats ────────────────────────────────────────────────────────────────
    int statY = cardY + 120;
    int statX = cardX + 30;
    int statSpacing = 35;
    int statFontSize = 18;

    // Score
    Color scoreColor = (state->adventure.levelFailed && level->goalType != GOAL_SCORE && level->goalType != GOAL_MIXED_ALL)
                       ? (Color){150, 150, 170, 255} : (Color){255, 255, 255, 255};
    sprintf(buf, "Puan: %d", state->score);
    GameDrawText(buf, statX, statY, statFontSize, scoreColor);
    if (level->goalType == GOAL_SCORE || level->goalType == GOAL_MIXED_ALL) {
        sprintf(buf, "Hedef: %d", state->adventure.goalScore);
        int tw = GameMeasureText(buf, statFontSize);
        GameDrawText(buf, cardX + cardW - 30 - tw, statY, statFontSize,
                     state->score >= state->adventure.goalScore ? (Color){50, 255, 100, 255} : (Color){255, 80, 80, 255});
    }

    // Diamonds
    if (level->goalType == GOAL_GEMS || level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
        statY += statSpacing;
        sprintf(buf, "Elmas: %d", state->adventure.collectedDiamonds);
        GameDrawText(buf, statX, statY, statFontSize, (Color){100, 200, 255, 255});
        sprintf(buf, "Hedef: %d", state->adventure.goalDiamonds);
        int tw = GameMeasureText(buf, statFontSize);
        GameDrawText(buf, cardX + cardW - 30 - tw, statY, statFontSize,
                     state->adventure.collectedDiamonds >= state->adventure.goalDiamonds ? (Color){50, 255, 100, 255} : (Color){255, 80, 80, 255});
    }

    // Emeralds
    if (level->goalType == GOAL_MIXED_GEMS || level->goalType == GOAL_MIXED_ALL) {
        statY += statSpacing;
        sprintf(buf, "Zumrut: %d", state->adventure.collectedEmeralds);
        GameDrawText(buf, statX, statY, statFontSize, (Color){50, 220, 100, 255});
        sprintf(buf, "Hedef: %d", state->adventure.goalEmeralds);
        int tw = GameMeasureText(buf, statFontSize);
        GameDrawText(buf, cardX + cardW - 30 - tw, statY, statFontSize,
                     state->adventure.collectedEmeralds >= state->adventure.goalEmeralds ? (Color){50, 255, 100, 255} : (Color){255, 80, 80, 255});
    }

    // ─── Buttons (vertical layout) ────────────────────────────────────────────
    const int btnW = 220, btnH = 50;
    const int btnGap = 15;
    const int btnX = (SCREEN_WIDTH - btnW) / 2;
    const int topBtnY = cardY + cardH - 140;
    const int botBtnY = topBtnY + btnH + btnGap;

    Rectangle btnTop = { btnX, topBtnY, btnW, btnH };
    Rectangle btnBot = { btnX, botBtnY, btnW, btnH };

    bool topHover = CheckCollisionPointRec(mouse, btnTop);
    bool botHover = CheckCollisionPointRec(mouse, btnBot);

    if (state->adventure.levelFailed) {
        // Top: Tekrar Dene | Bottom: Ana Menü
        Color topBg  = topHover ? (Color){80, 50, 120, 255} : (Color){55, 40, 85, 255};
        Color topBdr = topHover ? (Color){180, 120, 255, 255} : (Color){100, 70, 150, 255};
        DrawRectangleRounded(btnTop, 0.2f, 6, topBg);
        DrawRectangleRoundedLines(btnTop, 0.2f, 6, topBdr);
        const char *topText = "Tekrar Dene";
        int ttw = GameMeasureText(topText, 18);
        GameDrawText(topText, btnX + (btnW - ttw) / 2, topBtnY + (btnH - 18) / 2, 18,
                     topHover ? (Color){255, 255, 255, 255} : (Color){170, 150, 200, 255});

        Color botBg  = botHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255};
        Color botBdr = botHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255};
        DrawRectangleRounded(btnBot, 0.2f, 6, botBg);
        DrawRectangleRoundedLines(btnBot, 0.2f, 6, botBdr);
        const char *botText = "Ana Menu";
        int btw = GameMeasureText(botText, 18);
        GameDrawText(botText, btnX + (btnW - btw) / 2, botBtnY + (btnH - 18) / 2, 18,
                     botHover ? (Color){255, 255, 255, 255} : (Color){180, 180, 210, 255});
    } else {
        // Top: Sonraki Level | Bottom: Ana Menü
        Color topBg  = topHover ? (Color){50, 100, 60, 255} : (Color){35, 70, 45, 255};
        Color topBdr = topHover ? (Color){100, 220, 120, 255} : (Color){60, 140, 80, 255};
        DrawRectangleRounded(btnTop, 0.2f, 6, topBg);
        DrawRectangleRoundedLines(btnTop, 0.2f, 6, topBdr);
        const char *topText = (state->adventure.currentLevel + 1 < TOTAL_LEVELS) ? "Sonraki Level" : "Tamamlandi!";
        int ttw = GameMeasureText(topText, 18);
        GameDrawText(topText, btnX + (btnW - ttw) / 2, topBtnY + (btnH - 18) / 2, 18,
                     topHover ? (Color){255, 255, 255, 255} : (Color){150, 220, 170, 255});

        Color botBg  = botHover ? (Color){60, 60, 80, 255} : (Color){40, 40, 60, 255};
        Color botBdr = botHover ? (Color){150, 150, 180, 255} : (Color){80, 80, 100, 255};
        DrawRectangleRounded(btnBot, 0.2f, 6, botBg);
        DrawRectangleRoundedLines(btnBot, 0.2f, 6, botBdr);
        const char *botText = "Ana Menu";
        int btw = GameMeasureText(botText, 18);
        GameDrawText(botText, btnX + (btnW - btw) / 2, botBtnY + (btnH - 18) / 2, 18,
                     botHover ? (Color){255, 255, 255, 255} : (Color){180, 180, 210, 255});
    }

    // Hint at bottom
    const char *escHint = "ESC: Geri";
    int ehw = GameMeasureText(escHint, 14);
    GameDrawText(escHint, (SCREEN_WIDTH - ehw) / 2, SCREEN_HEIGHT - 25, 14, (Color){100, 100, 140, 200});
}
