#pragma once
#include "Bonus.hpp"

#include "IExpirable.hpp"

class AccelerateBallBonus : Bonus, IExpirable {
    static constexpr SDL_Color COLOR{255, 255, 0, 255};
    static constexpr float ACCELERATE_BALL_MULTIPLIER = 1.2f;
    static constexpr float DURATION = 5; //5s
public:
    AccelerateBallBonus(int x, int y) : Bonus(x, y), IExpirable(DURATION) {}

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


    ~AccelerateBallBonus() override = default;
};
