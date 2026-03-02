#include "../../include/systems/PhysicsSystem.h"
#include "../../include/utils/Constants.h"
#include <cmath>

void PhysicSystem::updatePlayer(Player& player, const std::vector<WindColumn>& winds, float deltaTime) {
    if (player.hp <= 0) return;

    player.previousY = player.position.y;

    if (player.hitTimer > 0) {
        player.hitTimer -= deltaTime;
    }
    if (player.dropThroughTimer > 0.0f) {
        player.dropThroughTimer -= deltaTime;
        if (player.dropThroughTimer < 0.0f) {
            player.dropThroughTimer = 0.0f;
        }
    }

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
        p.animTimer += deltaTime;

        float frameDuration = 0.08f;
        if (p.level == 4) {
            frameDuration = 0.1f;
        }

        if (p.animTimer >= frameDuration) {
            p.animTimer = 0.0f;
            p.currentFrame++;
            p.currentFrame %= p.maxFrames;
        }

        if (p.isStatic) continue; 

        if (p.level != 4 && !winds.empty()) {
            SDL_FRect bulletRect = p.getRect();
            for (const auto& wind : winds) {
                SDL_FRect windRect = wind.getRect();
                if (!SDL_HasRectIntersectionFloat(&bulletRect, &windRect)) {
                    continue;
                }

                float speed = std::sqrt((p.velocity.x * p.velocity.x) + (p.velocity.y * p.velocity.y));
                if (speed < 1.0f) {
                    break;
                }

                float bulletCenterX = p.position.x + p.radius;
                float windCenterX = windRect.x + windRect.w * 0.5f;
                float swirlDir = (bulletCenterX < windCenterX) ? 1.0f : -1.0f;

                float angle = std::atan2(p.velocity.y, p.velocity.x);
                float swirlTurn = swirlDir * 0.6f * deltaTime;
                float upTurn = -0.35f * deltaTime * (wind.force / GRAVITY);
                angle += swirlTurn + upTurn * 1.5;

                p.velocity.x = std::cos(angle) * speed;
                p.velocity.y = std::sin(angle) * speed;
                break;
            }
        }

        p.position.x += p.velocity.x * deltaTime;
        p.position.y += p.velocity.y * deltaTime;
    }
}

void PhysicSystem::updateExplosions(std::vector<Explosion>& explosions, float deltaTime) {
    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->animTimer += deltaTime;
        
        if (it->animTimer >= 0.07f) {
            it->animTimer = 0.0f;
            it->currentFrame++;
        }

        if (it->currentFrame >= it->maxFrames) {
            it->active = false;
        }

        if (!it->active) {
            it = explosions.erase(it);
        } else {
            ++it;
        }
    }
}