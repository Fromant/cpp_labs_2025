#pragma once
#include "Bonus.hpp"

class RandomBallDirectionBonus : Bonus {
    static constexpr SDL_Color COLOR{255, 0, 255, 255};

public:
    RandomBallDirectionBonus(int x, int y) : Bonus(x,y) {}

    void render(SDL_Renderer* renderer) override;

    void activate(GameState& game) override;
    void deactivate(GameState& game) override;

    ~RandomBallDirectionBonus() override = default;
};
