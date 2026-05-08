#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include "defs.h"
#include <stdbool.h>

// A single particle
typedef struct {
    bool active;
    Vector2 pos;
    Vector2 vel;       // velocity (px/s)
    float life;        // remaining life (seconds)
    float maxLife;
    float size;        // initial size (shrinks over time)
    Color color;
    float gravity;     // downward acceleration
} Particle;

// Particle system holding all active particles
typedef struct {
    Particle items[MAX_PARTICLES];
    int count;
} ParticleSystem;

// Emit N particles from position (x, y) with given color
void ParticleEmit(ParticleSystem *ps, float x, float y,
                  Color color, int count);

// Update all particles
void ParticleUpdate(ParticleSystem *ps, float dt);

// Draw all particles
void ParticleDraw(ParticleSystem *ps);

#endif // PARTICLE_H