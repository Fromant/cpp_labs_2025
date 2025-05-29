#pragma once
#include "GameObject.hpp"
#include "SDL3/SDL_pixels.h"
#include <memory>

class Bonus;

class Gem : public GameObject {
public:
    explicit Gem(SDL_Color c);
    Gem() = default;

    void Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const override;

    void SetBonus(std::unique_ptr<Bonus> b);
    bool HasBonus() const;
    void ActivateBonus(class Game* game, size_t index);

    SDL_Color color{255, 255, 255, 255};

private:
    std::unique_ptr<Bonus> bonus;
};