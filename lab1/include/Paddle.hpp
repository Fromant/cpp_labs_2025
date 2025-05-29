#pragma once

#include "GameState.hpp"
#include "bonuses/Bonus.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

constexpr float BASE_PADDLE_SPEED = 200.0f;

class Paddle : public ICollidable, public IRenderable {
    SDL_FRect rect;
    constexpr static SDL_Color COLOR{255, 255, 255, 255};
    constexpr static SDL_Color STICKY_COLOR{255, 255, 255, 255};

public:
    Paddle(float x, float y, float w, float h) : rect{x, y, w, h} {}

    void render(SDL_Renderer* renderer) override {
        SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_FRect& getCollisionRect() override {
        return rect;
    }

    void update(float deltaTime, GameState& state) {
        // Клавиатурный ввод для движения каретки
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);
        float moveSpeed = BASE_PADDLE_SPEED * state.paddleSpeedMultiplier * deltaTime;

        if (keyboardState[SDL_SCANCODE_LEFT]) {
            rect.x -= moveSpeed;
        }
        if (keyboardState[SDL_SCANCODE_RIGHT]) {
            rect.x += moveSpeed;
        }

        rect.w = state.paddleWidthMultiplier * BASE_PADDLE_WIDTH;


        // Ограничение движения каретки
        if (rect.x < 0) rect.x = 0;
        if (rect.x > SCREEN_WIDTH - rect.w) {
            rect.x = SCREEN_WIDTH - rect.w;
        }
    }

    void hit(GameState& gameState, SDL_FRect& collisionResult) override {}

    ~Paddle() override = default;
};
