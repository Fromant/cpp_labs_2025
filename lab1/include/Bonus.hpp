#pragma once
#include <SDL3/SDL.h>
#include "Block.hpp"

struct Bonus {
    SDL_FRect rect;
    BonusType type;
    bool active;
    float fallSpeed;

    Bonus(float x, float y, BonusType t);
    void Update(float deltaTime);
    void Activate(class Game& game);
};