#include "../../include/systems/InputSystem.h"
#include "../../include/utils/Constants.h"

void InputSystem::update(Player& player, float deltaTime, const bool* keys) {
    if (player.hp <= 0) return;

    if (checkUltimateCombo(player, keys)) {
        if (player.mana >= 100.0f && player.shootCooldown <= 0) {
            player.useUltimate = true;
            player.isCharging = false; 
            player.currentChargeTime = 0;   
            return;
        }
    }

    player.velocity.x = 0;
    SDL_Scancode dropKey = (player.id == 1) ? SDL_SCANCODE_S : SDL_SCANCODE_DOWN;

    if (player.id == 1) {
        if (keys[SDL_SCANCODE_A]) {
            player.velocity.x = -MOVE_SPEED;
            player.aimAngle = 180.0f; 
        }
        if (keys[SDL_SCANCODE_D]) {
            player.velocity.x = MOVE_SPEED;
            player.aimAngle = 0.0f; 
        }
        
        if (keys[SDL_SCANCODE_W] && player.isGrounded) {
            player.velocity.y = JUMP_FORCE;
            player.isGrounded = false;
        }
    } else {
        if (keys[SDL_SCANCODE_LEFT]) {
            player.velocity.x = -MOVE_SPEED;
            player.aimAngle = 180.0f;
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
            player.velocity.x = MOVE_SPEED;
            player.aimAngle = 0.0f;
        }

        if (keys[SDL_SCANCODE_UP] && player.isGrounded) {
            player.velocity.y = JUMP_FORCE;
            player.isGrounded = false;
        }
    }

    bool dropPressed = keys[dropKey];
    if (dropPressed && !player.dropKeyHeld && player.isGrounded) {
        player.dropThroughTimer = 0.18f;
        player.isGrounded = false;
        if (player.velocity.y < 220.0f) {
            player.velocity.y = 220.0f;
        }
    }
    player.dropKeyHeld = dropPressed;

    SDL_Scancode shootKey = (player.id == 1) ? SDL_SCANCODE_SPACE : SDL_SCANCODE_RETURN;

    if (keys[shootKey]) {
        if (player.mana >= MANA_COST) {
            player.isCharging = true;
            player.currentChargeTime += deltaTime;
            if (player.currentChargeTime > MAX_CHARGE_TIME) player.currentChargeTime = MAX_CHARGE_TIME;
        }
    } else {
        player.isCharging = false;
    }
}

bool InputSystem::checkUltimateCombo(const Player& player, const bool* keys) {
    if (player.id == 1) {
        return keys[SDL_SCANCODE_F];
    } else {
        return keys[SDL_SCANCODE_KP_0];
    }
}