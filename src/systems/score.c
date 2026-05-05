#include "score.h"
#include <stdio.h>

int ScoreCalculate(int linesCleared, int combo)
{
    if (linesCleared <= 0) return 0;
    return BASE_SCORE * linesCleared * combo;
}

int ScoreLoadHigh(void)
{
    int score = 0;
    FILE *f = fopen("data/highscore.dat", "rb");
    if (f) {
        fread(&score, sizeof(int), 1, f);
        fclose(f);
    }
    return score;
}

void ScoreSaveHigh(int score)
{
    FILE *f = fopen("data/highscore.dat", "wb");
    if (f) {
        fwrite(&score, sizeof(int), 1, f);
        fclose(f);
    }
}