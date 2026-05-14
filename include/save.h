#ifndef SAVE_H
#define SAVE_H

#include "defs.h"

int ScoreLoadHigh(void);
void ScoreSaveHigh(int score);

void NicknameLoad(char *out, int maxLen);
void NicknameSave(const char *name);

void ScoreboardLoad(int scores[10], char names[10][32], int *count);
void ScoreboardSave(int scores[10], char names[10][32], int count);
void ScoreboardAddEntry(int scores[10], char names[10][32], int *count,
                        const char *name, int score);

int LevelLoadProgress(void);
void LevelSaveProgress(int unlockedLevel);

#endif // SAVE_H
