#include "../../include/systems/PhysicsSystem.h"
#include "../../include/utils/Constants.h"

void PhysicSystem::updatePlayer(Player& player, const std::vector<WindColumn>& winds, float deltaTime) {
    if (player.hp <= 0) return;
    player.velocity.y += GRAVITY * deltaTime;

    SDL_FRect playerRect = player.getRect();
    for (const auto& wind : winds) {
        SDL_FRect windRect = wind.getRect();

        if (SDL_HasRectIntersectionFloat(&playerRect, &windRect)) {

            player.velocity.y -= wind.force * deltaTime;

            if (player.velocity.y < -wind.maxUpwardSpeed) {
                player.velocity.y = -wind.maxUpwardSpeed;
            }
        }
    }

    player.position.x += player.velocity.x * deltaTime;
    player.position.y += player.velocity.y * deltaTime;
}

void PhysicSystem::updateBullets(std::vector<Projectile>& bullets, const std::vector<WindColumn>& winds, float deltaTime) {
    for (auto& p : bullets) {
        if (!p.active) continue;
        
        p.existTime += deltaTime;

        SDL_FPoint bulletPoint = { p.position.x, p.position.y };
        
        for (const auto& wind : winds) {
            SDL_FRect windRect = wind.getRect();
            if (SDL_PointInRectFloat(&bulletPoint, &windRect)) {
                p.velocity.y -= wind.force * deltaTime;
                if (p.velocity.y < -wind.maxUpwardSpeed) {
                    p.velocity.y = -wind.maxUpwardSpeed;
                }
            }
        }

        p.position.x += p.velocity.x * deltaTime;
        p.position.y += p.velocity.y * deltaTime;
    }
}