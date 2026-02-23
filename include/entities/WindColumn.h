#pragma once
#include <SDL3/SDL.h>

class WindColumn {
public:
    SDL_FRect rect;
    float force;
    float maxUpwardSpeed;
    WindColumn(float x, float y, float w, float h, float f, float maxSpd);
    SDL_FRect getRect() const;
};