#ifndef TEXTURES_H
#define TEXTURES_H

#include "raylib.h"

// Central repository for all game textures.
// Loaded once at startup, unloaded at shutdown.
typedef struct {
    Texture2D crown;
    Texture2D diamond;
    Texture2D emerald;
    Texture2D gear;
    Texture2D home;
    Texture2D lock;
    Texture2D logout;
    Texture2D musicalNote;
    Texture2D waveSound;
    Texture2D replay;
    Texture2D completed;
    Texture2D logo;
} GameTextures;

// Global instance
extern GameTextures gameTextures;

// Load all game textures (call once at startup after InitWindow)
void TexturesLoad(void);

// Unload all game textures (call once at shutdown before CloseWindow)
void TexturesUnload(void);

#endif // TEXTURES_H