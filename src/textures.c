#include "textures.h"

// Global texture instance
GameTextures gameTextures;

void TexturesLoad(void)
{
    // Load all textures from PNG files
    gameTextures.crown = LoadTexture("assets/images/crown.png");
    gameTextures.diamond = LoadTexture("assets/images/diamond.png");
    gameTextures.emerald = LoadTexture("assets/images/emerald.png");
    gameTextures.gear = LoadTexture("assets/images/gear.png");
    gameTextures.home = LoadTexture("assets/images/home.png");
    gameTextures.lock = LoadTexture("assets/images/lock.png");
    gameTextures.logout = LoadTexture("assets/images/logout.png");
    gameTextures.musicalNote = LoadTexture("assets/images/musical-note.png");
    gameTextures.waveSound = LoadTexture("assets/images/wave-sound.png");
    gameTextures.replay = LoadTexture("assets/images/replay.png");
}

void TexturesUnload(void)
{
    UnloadTexture(gameTextures.crown);
    UnloadTexture(gameTextures.diamond);
    UnloadTexture(gameTextures.emerald);
    UnloadTexture(gameTextures.gear);
    UnloadTexture(gameTextures.home);
    UnloadTexture(gameTextures.lock);
    UnloadTexture(gameTextures.logout);
    UnloadTexture(gameTextures.musicalNote);
    UnloadTexture(gameTextures.waveSound);
    UnloadTexture(gameTextures.replay);
}