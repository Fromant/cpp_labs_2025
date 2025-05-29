#pragma once
#include "Bonus.hpp"
#include "IExpirable.hpp"

class SafetyNetBonus : Bonus, IExpirable {
    static constexpr SDL_Color COLOR{255, 165, 0, 255};
    static constexpr float DURATION = 3.0f; //3s

public:
    SafetyNetBonus(int x, int y) : Bonus(x, y), IExpirable(DURATION) {}

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

    ~SafetyNetBonus() override = default;
};
