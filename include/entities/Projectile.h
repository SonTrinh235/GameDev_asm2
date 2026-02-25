#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL3/SDL.h>
#include "../utils/Vector2.h"

class Projectile {
public:
    Projectile(float x, float y, float velX, float velY, int owner, float r = 5.0f, float dmg = 10.0f, int lvl = 1);

    Vector2 position;
    Vector2 velocity;
    float radius;
    int ownerId;
    float damage;
    bool active;
    int bounceCount;
    float existTime;
    
    int level;         
    int currentFrame;  
    float animTimer;   
    int maxFrames;
    bool isStatic;

    SDL_FRect getRect() const;
};

#endif