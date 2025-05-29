#pragma once
#include "Bonus.hpp"
#include "IExpirable.hpp"

class ShrinkPaddleBonus : Bonus, IExpirable {
    static constexpr SDL_Color COLOR{255, 0, 0, 255};
    static constexpr float SHRINK_PADDLE_MULTIPLIER = 1.2f;
    static constexpr float DURATION = 5.0f; //5s
    bool activated = false;

public:
    ShrinkPaddleBonus(int x, int y) : Bonus(x, y), IExpirable(DURATION) {}

    void render(SDL_Renderer* renderer) override;

    void update(float deltaTime, GameState& state) override {
        Bonus::update(deltaTime, state);
        if (activated) IExpirable::update(deltaTime, state);
    }

    void expire(GameState& state) override {
        deactivate(state);
    }

    void activate(GameState& game) override;
    void deactivate(GameState& game) override;

    ~ShrinkPaddleBonus() override = default;
};
