#include "adventure.h"
#include <stdio.h>
#include <time.h>

// ─── Life System ──────────────────────────────────────────────────────────────
void AdventureLoadLives(AdventureSaveData *save)
{
    FILE *f = fopen("data/adventure_lives.dat", "rb");
    if (f) {
        fread(&save->currentLives, sizeof(int), 1, f);
        fread(&save->lastLifeRegen, sizeof(time_t), 1, f);
        fclose(f);

        // Regen lives based on elapsed time
        AdventureRegenLives(save);
    } else {
        // Default: max lives
        save->currentLives = MAX_LIVES;
        save->lastLifeRegen = time(NULL);
    }
}

void AdventureSaveLives(AdventureSaveData *save)
{
    FILE *f = fopen("data/adventure_lives.dat", "wb");
    if (f) {
        fwrite(&save->currentLives, sizeof(int), 1, f);
        fwrite(&save->lastLifeRegen, sizeof(time_t), 1, f);
        fclose(f);
    }
}

void AdventureRegenLives(AdventureSaveData *save)
{
    if (save->currentLives >= MAX_LIVES) {
        save->currentLives = MAX_LIVES;
        save->lastLifeRegen = time(NULL);
        return;
    }

    time_t now = time(NULL);
    double elapsed = difftime(now, save->lastLifeRegen);
    int livesGained = (int)(elapsed / LIFE_REGEN_SEC);

    if (livesGained > 0) {
        save->currentLives += livesGained;
        if (save->currentLives > MAX_LIVES)
            save->currentLives = MAX_LIVES;
        save->lastLifeRegen = now;
        AdventureSaveLives(save);
    }
}