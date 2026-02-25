#include "../../include/entities/Projectile.h"

Projectile::Projectile(float x, float y, float velX, float velY, int owner, float r, float dmg, int lvl)
    : position(x, y), velocity(velX, velY), radius(r), ownerId(owner), damage(dmg), 
      active(true), bounceCount(0), existTime(0.0f), level(lvl), currentFrame(0), animTimer(0.0f) 
{
    isStatic = false;
    if (level == 4) {
        maxFrames = 6;
    } else {
        if (ownerId == 1) {
            if (level == 1) maxFrames = 2;
            else if (level == 2) maxFrames = 4;
            else maxFrames = 5;
        } else {
            maxFrames = 4;
        }
    }

    position.x -= radius;
    position.y -= radius;
}

SDL_FRect Projectile::getRect() const {
    return { position.x, position.y, radius * 2.0f, radius * 2.0f };
}