#pragma once
#include <SDL3/SDL.h>
#include "GameState.hpp"
#include "bonuses/Bonus.hpp"

constexpr float ACCELERATION_BLOCK_MULTIPLIER = 1.05f;

struct Block : ICollidable, IRenderable {
    SDL_FRect rect;
    int health;
    bool indestructible;
    bool accelerateBall;
    SDL_Color color;
    Bonus* innerBonus;

    Block(float x, float y, float w, float h, int hp, bool indestruct, bool accelerateBall, Bonus* bonus);
    bool IsDestroyed() const;

    void render(SDL_Renderer* renderer) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
        if (indestructible) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &rect);
        }
        if (accelerateBall) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderRect(renderer, &rect);
        }
    }

    SDL_FRect& getCollisionRect() override { return rect; };
    void hit(GameState& gameState, SDL_FRect& collisionResult) override;

    ~Block() override = default;
};
