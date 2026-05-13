#include "game.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PLAYER_FILE "data/player.txt"
#define SCOREBOARD_FILE "data/scoreboard.txt"

static void LoadPlayerFile(char *nickname, int maxLen, int *highScore)
{
    char line[128];
    FILE *f = fopen(PLAYER_FILE, "r");

    if (nickname && maxLen > 0) nickname[0] = '\0';
    if (highScore) *highScore = 0;
    if (!f) return;

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "nickname=", 9) == 0 && nickname && maxLen > 0) {
            line[strcspn(line, "\r\n")] = '\0';
            strncpy(nickname, line + 9, maxLen - 1);
            nickname[maxLen - 1] = '\0';
        } else if (strncmp(line, "highScore=", 10) == 0 && highScore) {
            int value = atoi(line + 10);
            if (value >= 0) *highScore = value;
        }
    }

    fclose(f);
}

static void SavePlayerFile(const char *nickname, int highScore)
{
    FILE *f = fopen(PLAYER_FILE, "w");
    if (!f) return;

    fprintf(f, "# Player save file. You can edit this by hand.\n");
    fprintf(f, "nickname=%s\n", nickname ? nickname : "");
    fprintf(f, "highScore=%d\n", highScore);
    fclose(f);
}

// ─── High Score Save/Load (moved from deleted score module) ──────────────────
int ScoreLoadHigh(void)
{
    int highScore = 0;
    LoadPlayerFile(NULL, 0, &highScore);
    return highScore;
}

void ScoreSaveHigh(int score)
{
    char nickname[32];
    LoadPlayerFile(nickname, 32, NULL);
    SavePlayerFile(nickname, score);
}

// ─── Nickname Save/Load ──────────────────────────────────────────────────────
void NicknameLoad(char *out, int maxLen)
{
    LoadPlayerFile(out, maxLen, NULL);
}

void NicknameSave(const char *name)
{
    int highScore = ScoreLoadHigh();
    SavePlayerFile(name, highScore);
}

// ─── Scoreboard Save/Load ────────────────────────────────────────────────────
void ScoreboardLoad(int scores[10], char names[10][32], int *count)
{
    *count = 0;
    for (int i = 0; i < 10; i++) {
        scores[i] = 0;
        names[i][0] = '\0';
    }

    FILE *f = fopen(SCOREBOARD_FILE, "r");
    if (f) {
        char line[128];
        while (*count < 10 && fgets(line, sizeof(line), f)) {
            int score = 0;
            char name[32] = "";
            char *sep;

            if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
                continue;

            sep = strchr(line, '|');
            if (sep) {
                *sep = '\0';
                score = atoi(line);
                strncpy(name, sep + 1, 31);
                name[strcspn(name, "\r\n")] = '\0';
            } else if (sscanf(line, "%d %31[^\r\n]", &score, name) != 2) {
                continue;
            }

            if (score < 0 || name[0] == '\0')
                continue;

            scores[*count] = score;
            strncpy(names[*count], name, 31);
            names[*count][31] = '\0';
            (*count)++;
        }
        fclose(f);
    }
}

void ScoreboardSave(int scores[10], char names[10][32], int count)
{
    FILE *f = fopen(SCOREBOARD_FILE, "w");
    if (f) {
        fprintf(f, "# score|name\n");
        for (int i = 0; i < count; i++) {
            fprintf(f, "%d|%s\n", scores[i], names[i]);
        }
        fclose(f);
    }
}

void ScoreboardAddEntry(int scores[10], char names[10][32], int *count, const char *name, int score)
{
    // Check if this name already exists in the scoreboard
    for (int i = 0; i < *count; i++) {
        if (strcmp(names[i], name) == 0) {
            // Name exists: update score if new score is higher
            if (score > scores[i]) {
                scores[i] = score;
                // Re-sort: bubble this entry up if needed
                while (i > 0 && scores[i] > scores[i - 1]) {
                    int tmpScore = scores[i];
                    scores[i] = scores[i - 1];
                    scores[i - 1] = tmpScore;
                    char tmpName[32];
                    strcpy(tmpName, names[i]);
                    strcpy(names[i], names[i - 1]);
                    strcpy(names[i - 1], tmpName);
                    i--;
                }
            }
            ScoreboardSave(scores, names, *count);
            return;
        }
    }

    // New name: find insertion position (sort descending)
    int pos = *count;
    for (int i = 0; i < *count; i++) {
        if (score > scores[i]) {
            pos = i;
            break;
        }
    }

    // Shift entries down if needed
    if (pos < 10) {
        int moveCount = (*count < 10) ? *count : 9;
        for (int i = moveCount; i > pos; i--) {
            scores[i] = scores[i - 1];
            strcpy(names[i], names[i - 1]);
        }
        scores[pos] = score;
        strncpy(names[pos], name, 31);
        names[pos][31] = '\0';
        if (*count < 10) (*count)++;
    }

    ScoreboardSave(scores, names, *count);
}

// ─── Game Init ───────────────────────────────────────────────────────────────
void GameInit(GameState *state)
{
    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    LevelLoadDefinitions();
    BoardInit(&state->board);
    state->highScore = ScoreLoadHigh();
    state->selectedLevel = 0;

    // Load nickname
    NicknameLoad(state->nickname, 32);
    state->nicknameInput[0] = '\0';
    state->nicknameCursorPos = 0;

    // Load scoreboard
    ScoreboardLoad(state->scoreboardScores, state->scoreboardNames, &state->scoreboardCount);

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        SlotClear(&state->slots[i]);

    // Load level progress
    LevelLoadProgress(state->levelCompleted);
}

// ─── Game Reset (unified classic + adventure) ────────────────────────────────
void GameReset(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        SlotClear(&state->slots[i]);

    // Initialize level (sets up board + prefill)
    LevelInit(&state->level, state->selectedLevel, &state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->anims.count = 0;
    state->floatTexts.count = 0;
    state->particles.count = 0;

    // Determine gem chances from level definition
    const LevelDef *def = &LevelGetDefs()[state->selectedLevel];
    float diamondChance = (def->targetDiamonds > 0) ? DIAMOND_SPAWN_CHANCE : 0.0f;
    float emeraldChance = (def->targetEmeralds > 0) ? EMERALD_SPAWN_CHANCE : 0.0f;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH, diamondChance, emeraldChance);
}
