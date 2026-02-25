#include "../../include/entities/Player.h"
#include "../../include/utils/Constants.h"

Player::Player(int playerId, float startX, float startY) 
    : id(playerId), 
      position(startX, startY), 
      velocity(0, 0), 
      width(65.5), height(65.5),
      isGrounded(false), 
      mana(MAX_MANA), maxMana(MAX_MANA), 
      shootCooldown(0),
      currentChargeTime(0.0f), isCharging(false),
      aimAngle(0.0f),
      hp(100.0f), maxHp(100.0f),
      shieldTimer(0.0f), infiniteManaTimer(0.0f),
      hitTimer(0.0f)
{
    useUltimate = false;
    if (id == 1) { 
        r = 255; g = 50; b = 50; aimAngle = 0.0f; 
    }    
    else if (id == 2) { 
        r = 50; g = 50; b = 255; aimAngle = 180.0f; 
    }  
}

SDL_FRect Player::getRect() const { 
    return { position.x, position.y, width, height }; 
}