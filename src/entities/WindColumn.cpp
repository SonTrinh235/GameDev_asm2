#include "../../include/entities/WindColumn.h"

WindColumn::WindColumn(float x, float y, float w, float h, float f, float maxSpd)
    : force(f), maxUpwardSpeed(maxSpd) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

SDL_FRect WindColumn::getRect() const {
    return rect;
}