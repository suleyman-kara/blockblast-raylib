#include "particle.h"
#include <stdlib.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979f
#endif

void ParticleEmit(ParticleSystem *ps, float x, float y,
                  Color color, int count)
{
    for (int i = 0; i < count; i++) {
        if (ps->count >= MAX_PARTICLES) return;

        Particle *p = &ps->items[ps->count++];
        p->active = true;
        p->pos = (Vector2){x, y};

        // Random direction and speed
        float angle = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
        float speed = 80.0f + ((float)rand() / (float)RAND_MAX) * 160.0f;
        p->vel = (Vector2){cosf(angle) * speed, sinf(angle) * speed};

        p->life = PARTICLE_LIFE + ((float)rand() / (float)RAND_MAX) * 0.4f;
        p->maxLife = p->life;
        p->size = 3.0f + ((float)rand() / (float)RAND_MAX) * 4.0f;
        p->gravity = 200.0f + ((float)rand() / (float)RAND_MAX) * 100.0f;

        // Slight color variation
        p->color = color;
        int variation = (rand() % 40) - 20;
        p->color.r = (unsigned char)fminf(255, fmaxf(0, color.r + variation));
        p->color.g = (unsigned char)fminf(255, fmaxf(0, color.g + variation));
    }
}

void ParticleUpdate(ParticleSystem *ps, float dt)
{
    bool anyActive = false;

    for (int i = 0; i < ps->count; i++) {
        Particle *p = &ps->items[i];
        if (!p->active) continue;

        p->life -= dt;
        if (p->life <= 0.0f) {
            p->active = false;
            continue;
        }

        anyActive = true;

        // Apply gravity
        p->vel.y += p->gravity * dt;

        // Move
        p->pos.x += p->vel.x * dt;
        p->pos.y += p->vel.y * dt;
    }

    // Reset count if all inactive
    if (!anyActive) ps->count = 0;
}

void ParticleDraw(ParticleSystem *ps)
{
    for (int i = 0; i < ps->count; i++) {
        Particle *p = &ps->items[i];
        if (!p->active) continue;

        float t = p->life / p->maxLife;  // 1.0 -> 0.0

        // Size shrinks over time
        float currentSize = p->size * t;
        if (currentSize < 0.5f) continue;

        // Alpha fades
        Color clr = p->color;
        clr.a = (unsigned char)(t * 255.0f);

        DrawRectangle(
            (int)(p->pos.x - currentSize / 2),
            (int)(p->pos.y - currentSize / 2),
            (int)currentSize + 1,
            (int)currentSize + 1,
            clr
        );
    }
}