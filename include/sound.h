#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include <stdbool.h>

// Sound system — holds all loaded audio resources and volume state
typedef struct {
    // Sound effects
    Sound sfxPlace;       // block-placing.ogg
    Sound sfxLineClear1;  // line-clear-1.ogg
    Sound sfxLineClear2;  // line-clear-2.ogg
    Sound sfxCombo1;      // combo-1.ogg
    Sound sfxCombo2;      // combo-2.ogg
    Sound sfxLose;        // lose.ogg
    Sound sfxMenuClick;   // menu-click.ogg

    // Background music
    Music bgMusic;        // background-music.wav

    // Toggle state
    bool sfxEnabled;
    bool musicEnabled;
} SoundSystem;

// Initialize audio device and load all sound resources
void SoundInit(SoundSystem *snd);

// Unload all sound resources and close audio device
void SoundClose(SoundSystem *snd);

// Must be called every frame to keep music stream playing
void SoundUpdate(SoundSystem *snd);

// Toggle functions
void SoundToggleSfx(SoundSystem *snd);
void SoundToggleMusic(SoundSystem *snd);

// Play specific sound effects (respects sfxEnabled)
void SoundPlayPlace(SoundSystem *snd);
void SoundPlayLineClear(SoundSystem *snd, int linesCleared);
void SoundPlayCombo(SoundSystem *snd, int comboCount);
void SoundPlayLose(SoundSystem *snd);
void SoundPlayMenuClick(SoundSystem *snd);

#endif // SOUND_H
