#pragma once
#include <vector>
#include "../../include/entities/Player.h"
#include "../../include/entities/Projectile.h"
#include "../../include/entities/WindColumn.h"
#include "CollisionSystem.h" // Để nhận diện struct Explosion

class PhysicSystem {
public:
    void updatePlayer(Player& player, const std::vector<WindColumn>& winds, float deltaTime);
    void updateBullets(std::vector<Projectile>& bullets, const std::vector<WindColumn>& winds, float deltaTime);
    void updateExplosions(std::vector<Explosion>& explosions, float deltaTime);
};