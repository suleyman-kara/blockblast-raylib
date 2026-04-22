#include "sound.h"
#include <stdio.h>

// Path prefix for sound assets (relative to working directory)
#define SND_PATH "../assets/sounds/"

void SoundInit(SoundSystem *snd)
{
    InitAudioDevice();

    // Load sound effects
    snd->sfxPlace      = LoadSound(SND_PATH "block-placing.ogg");
    snd->sfxLineClear1 = LoadSound(SND_PATH "line-clear-1.ogg");
    snd->sfxLineClear2 = LoadSound(SND_PATH "line-clear-2.ogg");
    snd->sfxCombo1     = LoadSound(SND_PATH "combo-1.ogg");
    snd->sfxCombo2     = LoadSound(SND_PATH "combo-2.ogg");
    snd->sfxLose       = LoadSound(SND_PATH "lose.ogg");
    snd->sfxMenuClick  = LoadSound(SND_PATH "menu-click.ogg");

    // Load background music
    snd->bgMusic = LoadMusicStream(SND_PATH "background-music.wav");
    snd->bgMusic.looping = true;

    // Default: both enabled
    snd->sfxEnabled   = true;
    snd->musicEnabled = true;

    // Set default volumes
    SetSoundVolume(snd->sfxPlace,      0.7f);
    SetSoundVolume(snd->sfxLineClear1, 0.8f);
    SetSoundVolume(snd->sfxLineClear2, 0.8f);
    SetSoundVolume(snd->sfxCombo1,     0.9f);
    SetSoundVolume(snd->sfxCombo2,     0.9f);
    SetSoundVolume(snd->sfxLose,       0.8f);
    SetSoundVolume(snd->sfxMenuClick,  0.6f);
    SetMusicVolume(snd->bgMusic, 0.4f);

    // Start playing background music
    PlayMusicStream(snd->bgMusic);
}

void SoundClose(SoundSystem *snd)
{
    // Unload sound effects
    UnloadSound(snd->sfxPlace);
    UnloadSound(snd->sfxLineClear1);
    UnloadSound(snd->sfxLineClear2);
    UnloadSound(snd->sfxCombo1);
    UnloadSound(snd->sfxCombo2);
    UnloadSound(snd->sfxLose);
    UnloadSound(snd->sfxMenuClick);

    // Unload music
    UnloadMusicStream(snd->bgMusic);

    CloseAudioDevice();
}

void SoundUpdate(SoundSystem *snd)
{
    if (snd->musicEnabled) {
        UpdateMusicStream(snd->bgMusic);
    }
}

void SoundToggleSfx(SoundSystem *snd)
{
    snd->sfxEnabled = !snd->sfxEnabled;
}

void SoundToggleMusic(SoundSystem *snd)
{
    snd->musicEnabled = !snd->musicEnabled;
    if (snd->musicEnabled) {
        ResumeMusicStream(snd->bgMusic);
    } else {
        PauseMusicStream(snd->bgMusic);
    }
}

void SoundPlayPlace(SoundSystem *snd)
{
    if (snd->sfxEnabled) PlaySound(snd->sfxPlace);
}

void SoundPlayLineClear(SoundSystem *snd, int linesCleared)
{
    if (!snd->sfxEnabled) return;
    // Use the second (bigger) clear sound for 2+ lines
    if (linesCleared >= 2)
        PlaySound(snd->sfxLineClear2);
    else
        PlaySound(snd->sfxLineClear1);
}

void SoundPlayCombo(SoundSystem *snd, int comboCount)
{
    if (!snd->sfxEnabled) return;
    // Use the second (epic) combo sound for combo >= 4
    if (comboCount >= 4)
        PlaySound(snd->sfxCombo2);
    else
        PlaySound(snd->sfxCombo1);
}

void SoundPlayLose(SoundSystem *snd)
{
    if (snd->sfxEnabled) PlaySound(snd->sfxLose);
}

void SoundPlayMenuClick(SoundSystem *snd)
{
    if (snd->sfxEnabled) PlaySound(snd->sfxMenuClick);
}
