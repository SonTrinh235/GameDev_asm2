#pragma once
#include <SDL3/SDL.h>
#include "../entities/Player.h"

class InputSystem {
public:
    void update(Player& player, float deltaTime, const bool* keys);

private:
    bool checkUltimateCombo(const Player& player, const bool* keys);
};