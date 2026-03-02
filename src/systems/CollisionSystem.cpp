#include "../../include/systems/CollisionSystem.h"
#include "../../include/utils/Constants.h"
#include "../../include/entities/Explosion.h"
#include <algorithm>
#include <cmath>

void CollisionSystem::update(Player& player, std::vector<Projectile>& bullets, 
                            const std::vector<Platform>& platforms, 
                            std::vector<Item>& items, 
                            std::vector<Explosion>& explosions) {
    if (player.hp <= 0) return;

    player.isGrounded = false;
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.width) player.position.x = SCREEN_WIDTH - player.width;
    // if (player.position.y > SCREEN_HEIGHT - player.height) {
    //     player.position.y = SCREEN_HEIGHT - player.height;
    //     player.velocity.y = 0;
    //     player.isGrounded = true;
    // }

    for (const auto& plat : platforms) {
        SDL_FRect platRect = plat.getRect();
        if (player.dropThroughTimer <= 0.0f && player.velocity.y >= 0) {
            bool collisionX = (player.position.x + player.width > plat.x) && (player.position.x < plat.x + plat.width);
            bool feetHitting = (player.position.y + player.height >= plat.y) && (player.position.y + player.height <= plat.y + 15.0f);
            if (collisionX && feetHitting) {
                player.position.y = plat.y - player.height;
                player.velocity.y = 0;
                player.isGrounded = true;
            }
        }
    }

    SDL_FRect playerRect = player.getRect();
    
    // --- Logic Items ---
    for (auto& item : items) {
        if (!item.active) continue;
        
        SDL_FRect itemRect = item.getRect();
        if (SDL_HasRectIntersectionFloat(&playerRect, &itemRect)) {
            item.active = false;
            if (item.type == ITEM_HEAL) player.hp = std::min(player.maxHp, player.hp + 30.0f);
            else if (item.type == ITEM_SHIELD) player.shieldTimer = 5.0f;
            else if (item.type == ITEM_INFINITE_MANA) player.infiniteManaTimer = 7.0f;
        }
    }

    // --- Logic Projectiles ---
    for (size_t i = 0; i < bullets.size(); ++i) {
        auto& p = bullets[i];
        if (!p.active) continue;
        if (p.existTime < 0.02f) continue; 

        SDL_FRect bulletRect = p.getRect();

        if (p.level == 4) {
            for (size_t j = i + 1; j < bullets.size(); ++j) {
                auto& other = bullets[j];
                if (other.active && other.level == 4 && p.ownerId != other.ownerId) {
                    SDL_FRect otherRect = other.getRect();
                    if (SDL_HasRectIntersectionFloat(&bulletRect, &otherRect)) {
                        float pCenterX = p.position.x + p.radius;
                        float pCenterY = p.position.y + p.radius;
                        float oCenterX = other.position.x + other.radius;
                        float oCenterY = other.position.y + other.radius;

                        float blastX = (pCenterX + oCenterX) / 2.0f;
                        float blastY = (pCenterY + oCenterY) / 2.0f;
                        
                        explosions.emplace_back(blastX, blastY);
                        
                        p.active = false;
                        other.active = false;
                        break;
                    }
                }
            }
        }

        if (!p.active) continue;

        if (SDL_HasRectIntersectionFloat(&bulletRect, &playerRect)) {
            if (p.ownerId != player.id) {
                if (player.shieldTimer <= 0.0f) {
                    player.hp -= p.damage;
                    player.hitTimer = 0.2f;
                    if (player.hp < 0) player.hp = 0;
                }
                if (p.level != 4) {
                    p.active = false;
                    continue; 
                }
            }
        }

        bool bouncedThisFrame = false;
        if (p.position.x <= 0 || p.position.x >= SCREEN_WIDTH - p.radius * 2) {
            if (p.level == 4) { p.active = false; continue; }
            p.velocity.x = -p.velocity.x;
            p.position.x = std::clamp(p.position.x, 0.1f, SCREEN_WIDTH - p.radius * 2 - 0.1f);
            bouncedThisFrame = true;
        }

        if (p.position.y <= 0) {
            if (p.level == 4) { p.active = false; continue; }
            p.velocity.y = -p.velocity.y;
            p.position.y = 0.1f;
            bouncedThisFrame = true;
        } else if (p.position.y >= SCREEN_HEIGHT) {
            p.active = false;
            continue;
        }

        if (p.level == 4) continue;

        for (const auto& plat : platforms) {
            SDL_FRect platRect = plat.getRect();
            SDL_FRect inter;
            if (SDL_GetRectIntersectionFloat(&bulletRect, &platRect, &inter)) {
                float bulletCenterX = p.position.x + p.radius;
                float bulletCenterY = p.position.y + p.radius;
                float platCenterX = platRect.x + platRect.w / 2.0f;
                float platCenterY = platRect.y + platRect.h / 2.0f;

                if (inter.w < inter.h) {
                    p.velocity.x = -p.velocity.x;
                    p.position.x = (bulletCenterX < platCenterX) ? platRect.x - p.radius * 2 - 0.1f : platRect.x + platRect.w + 0.1f;
                } else {
                    p.velocity.y = -p.velocity.y;
                    p.position.y = (bulletCenterY < platCenterY) ? platRect.y - p.radius * 2 - 0.1f : platRect.y + platRect.h + 0.1f;
                }
                bouncedThisFrame = true;
                break;
            }
        }

        if (bouncedThisFrame) {
            p.bounceCount++;
            if (p.bounceCount >= MAX_BOUNCES) p.active = false;
        }
    }
}