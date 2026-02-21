#include "../../include/systems/CollisionSystem.h"
#include "../../include/utils/Constants.h"

void CollisionSystem::update(Player& player, std::vector<Projectile>& bullets, const std::vector<Platform>& platforms) {
    if (player.hp <= 0) return;

    player.isGrounded = false;
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.width) player.position.x = SCREEN_WIDTH - player.width;
    if (player.position.y > SCREEN_HEIGHT - player.height) {
        player.position.y = SCREEN_HEIGHT - player.height;
        player.velocity.y = 0;
        player.isGrounded = true;
    }

    for (const auto& plat : platforms) {
        bool collisionX = (player.position.x + player.width > plat.x) && (player.position.x < plat.x + plat.width);
        bool falling = player.velocity.y >= 0;
        bool feetHitting = (player.position.y + player.height >= plat.y) && (player.position.y + player.height <= plat.y + 20);
        if (collisionX && falling && feetHitting) {
            player.position.y = plat.y - player.height;
            player.velocity.y = 0;
            player.isGrounded = true;
        }
    }

    for (auto& p : bullets) {
        if (!p.active) continue;
        if (p.existTime < 0.05f) continue;

        SDL_FRect bulletRect = p.getRect();
        SDL_FRect playerRect = player.getRect();

        if (SDL_HasRectIntersectionFloat(&bulletRect, &playerRect)) {
            if (p.ownerId != player.id) {
                player.hp -= p.damage;
                if (player.hp < 0) player.hp = 0;
                p.active = false;
                continue; 
            }
        }

        if (player.id == 1) {
            bool hasBounced = false;

            if (p.position.x <= 0) { 
                p.position.x = 0.1f; p.velocity.x = -p.velocity.x; hasBounced = true; 
            } else if (p.position.x >= SCREEN_WIDTH - p.radius * 2) { 
                p.position.x = (SCREEN_WIDTH - p.radius * 2) - 0.1f; p.velocity.x = -p.velocity.x; hasBounced = true; 
            }
            
            if (p.position.y <= 0) { 
                p.position.y = 0.1f; p.velocity.y = -p.velocity.y; hasBounced = true; 
            } else if (p.position.y >= SCREEN_HEIGHT - p.radius * 2) { 
                p.position.y = (SCREEN_HEIGHT - p.radius * 2) - 0.1f; p.velocity.y = -p.velocity.y; hasBounced = true; 
            }

            for (const auto& plat : platforms) {
                SDL_FRect platRect = plat.getRect();
                SDL_FRect intersection;
                if (SDL_GetRectIntersectionFloat(&bulletRect, &platRect, &intersection)) {
                    if (intersection.w < intersection.h) {
                        p.velocity.x = -p.velocity.x;
                        if (p.position.x < plat.x) p.position.x -= intersection.w;
                        else p.position.x += intersection.w;
                    } else {
                        p.velocity.y = -p.velocity.y;
                        if (p.position.y < plat.y) p.position.y -= intersection.h;
                        else p.position.y += intersection.h;
                    }
                    hasBounced = true;
                    break;
                }
            }

            if (hasBounced) {
                p.bounceCount++;
                if (p.bounceCount >= MAX_BOUNCES) p.active = false;
            }
        }
    }
}