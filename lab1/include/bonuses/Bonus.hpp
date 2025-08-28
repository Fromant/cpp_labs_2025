#pragma once
#include <SDL3/SDL.h>

#include "GameState.hpp"
#include "IRenderable.hpp"
#include "ICollidable.hpp"

constexpr float fallSpeed = 100; //pixels/sec
constexpr int BONUS_WIDTH = 40;
constexpr int BONUS_HEIGHT = 40;

class Bonus : public IRenderable, public ICollidable {
protected:
    SDL_FRect rect;
    bool active = true;

public:
    Bonus(float x, float y);
    bool doRender = false;

    SDL_FRect& getCollisionRect() override { return rect; }

    void hit(GameState& game, SDL_FRect& collisionResult) override {
        activate(game);
        disableRendering();
    }

    virtual void enableRendering() {
        doRender = true;
    }

    virtual void disableRendering() {
        doRender = false;
    }

    virtual bool isActive() const { return active; }

    void render(SDL_Renderer* renderer) override = 0;
    virtual void update(float deltaTime, GameState& state);
    virtual void activate(GameState& game) = 0;
    virtual void deactivate(GameState& game) = 0;

    ~Bonus() override = default;
};
