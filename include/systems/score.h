#ifndef SCORE_H
#define SCORE_H

#include "core/defs.h"

// Add score: base * linesCleared * comboMultiplier
int ScoreCalculate(int linesCleared, int combo);

// Load high score from file. Returns 0 if file doesn't exist.
int ScoreLoadHigh(void);

// Save high score to file
void ScoreSaveHigh(int score);

#endif // SCORE_H