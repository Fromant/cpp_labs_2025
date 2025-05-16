#pragma once
#include <SDL3/SDL.h>

enum class BonusType {
    None,
    ExpandPaddle,
    ShrinkPaddle,
    SpeedUp,
    StickyPaddle,
    SafetyNet,
    RandomDirection
};

struct Block {
    SDL_FRect rect;
    int health;
    bool indestructible;
    BonusType bonus;
    bool hasBonus;
    SDL_Color color;

    Block(float x, float y, float w, float h, int hp, bool indestruct, BonusType bonusType);
    void Hit();
    bool IsDestroyed() const;
};