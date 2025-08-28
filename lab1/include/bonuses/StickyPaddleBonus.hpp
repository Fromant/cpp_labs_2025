#pragma once

#include "Bonus.hpp"
#include "IExpirable.hpp"

class StickyPaddleBonus : Bonus, IExpirable {
    static constexpr SDL_Color COLOR{0, 255, 0, 255};
    static constexpr float DURATION = 4.0f; //4s

public:
    StickyPaddleBonus(int x, int y) : Bonus(x, y), IExpirable(DURATION) {}

    void render(SDL_Renderer* renderer) override;

    void update(float deltaTime, GameState& state) override;

    void expire(GameState& state) override;

    void activate(GameState& game) override;
    void deactivate(GameState& game) override;

    ~StickyPaddleBonus() override = default;
};
