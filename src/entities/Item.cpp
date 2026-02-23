#include "../../include/entities/Item.h"

Item::Item(float _x, float _y, ItemType _type) 
    : x(_x), y(_y), width(24.0f), height(24.0f), type(_type), active(true), floatTimer(0.0f) 
{}

SDL_FRect Item::getRect() const {
    return { x, y, width, height };
}