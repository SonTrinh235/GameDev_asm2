#pragma once
#include <SDL3/SDL.h>

enum ItemType {
    ITEM_HEAL,
    ITEM_SHIELD,
    ITEM_INFINITE_MANA
};

struct Item {
    float x, y;
    float width, height;
    ItemType type;
    bool active;
    float floatTimer;

    Item(float _x, float _y, ItemType _type);
    SDL_FRect getRect() const;
};