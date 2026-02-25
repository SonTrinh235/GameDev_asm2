#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "../entities/Player.h"
#include "../entities/Projectile.h"
#include "../entities/Platform.h"
#include "../entities/Item.h"
#include "../entities/Explosion.h"


class CollisionSystem {
public:
    void update(Player& player, std::vector<Projectile>& bullets, 
                const std::vector<Platform>& platforms, 
                std::vector<Item>& items, 
                std::vector<Explosion>& explosions);
};