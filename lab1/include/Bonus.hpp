#pragma once
#include "GameObject.hpp"
#include <memory>

enum class BonusType { RECOLOR, BOMB };

class Bonus : public GameObject {
public:
    virtual void Activate(class Game* game, size_t index) = 0;
    virtual ~Bonus() = default;
};

class RecolorBonus : public Bonus {
public:
    void Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const override;
    void Activate(class Game* game, size_t index) override;
};

class BombBonus : public Bonus {
public:
    void Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const override;
    void Activate(class Game* game, size_t index) override;
};