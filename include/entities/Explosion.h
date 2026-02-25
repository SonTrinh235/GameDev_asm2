#pragma once
#include <SDL3/SDL.h>

struct Explosion {
    float x, y; 
    int currentFrame; 
    float animTimer;  
    int maxFrames;
    bool active;

    Explosion(float xPos, float yPos) 
        : x(xPos), y(yPos), currentFrame(0), animTimer(0.0f), maxFrames(7), active(true) {}
};