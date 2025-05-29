#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>

#include "ICollidable.hpp"
#include "IRenderable.hpp"
#include "Paddle.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

struct GameState;

class Ball : public IRenderable, public ICollidable {
    SDL_FRect rect;
    constexpr static SDL_Color COLOR{255, 255, 255, 255};

public:
    Ball(float x, float y, float w, float h) : rect{x, y, w, h} {}

    void render(SDL_Renderer* renderer) override {
        SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_FRect& getCollisionRect() override {
        return rect;
    }

    bool checkCollisionWith(ICollidable& other, GameState& state) override {
        bool r = ICollidable::checkCollisionWith(other, state);
        if (!r) return false;
        if (dynamic_cast<Paddle*>(&other)) {
            if (!state.stickyPaddle) {
                float hitPosition = (rect.x + rect.w / 2 - other.getCollisionRect().x) / other.getCollisionRect().w -
                    0.5f;
                float prevVelocity = sqrtf(
                    state.ballVelocity.x * state.ballVelocity.x + state.ballVelocity.y * state.ballVelocity.y);
                state.ballVelocity.x = hitPosition * prevVelocity;
                state.ballVelocity.y = -
                    sqrtf(prevVelocity * prevVelocity - state.ballVelocity.x * state.ballVelocity.x);
            }
            else {
                state.ballSticked = true;
                state.stickyPaddle = false;
            }
        }
        return true;
    }

    void hit(GameState& gameState, SDL_FRect& collisionResult) override {
        float overlapLeft = rect.x + rect.w - collisionResult.x;
        float overlapRight = collisionResult.x + collisionResult.w - rect.x;
        float overlapTop = rect.y + rect.h - collisionResult.y;
        float overlapBottom = collisionResult.y + collisionResult.h - rect.y;

        bool horizontalCollision = std::min(overlapLeft, overlapRight) <
            std::min(overlapTop, overlapBottom);

        if (horizontalCollision) {
            gameState.ballVelocity.x = (overlapLeft < overlapRight)
                                           ? -std::fabs(gameState.ballVelocity.x)
                                           : fabs(gameState.ballVelocity.x);
            rect.x += (overlapLeft < overlapRight) ? -collisionResult.w : collisionResult.w;
        }
        else {
            gameState.ballVelocity.y = (overlapTop < overlapBottom)
                                           ? -fabs(gameState.ballVelocity.y)
                                           : fabs(gameState.ballVelocity.y);
            rect.y += (overlapTop < overlapBottom) ? -collisionResult.h : collisionResult.h;
        }
    }

    bool update(float deltaTime, GameState& state, Paddle& paddle) {
        if (state.ballSticked) {
            rect.x = paddle.getCollisionRect().x + paddle.getCollisionRect().w / 2 - rect.w / 2;
            rect.y = paddle.getCollisionRect().y - rect.h;
            return false;
        }

        rect.x += state.ballVelocity.x * deltaTime * state.ballSpeedMultiplier;
        rect.y += state.ballVelocity.y * deltaTime * state.ballSpeedMultiplier;

        // Проверка поражения
        if (rect.y + rect.h > SCREEN_HEIGHT) {
            if (state.safetyNetActive) {
                // Отскок от защитной сетки
                state.ballVelocity.y = -fabs(state.ballVelocity.y);
                rect.y = SCREEN_HEIGHT - 2 * BASE_BALL_HEIGHT;
                state.safetyNetActive = false;
            }
            else {
                state.lives--;
                if (state.lives > 0) {
                    // Респавн мяча
                    rect = {
                            SCREEN_WIDTH / 2 - BASE_BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BASE_BALL_HEIGHT / 2,
                            BASE_BALL_WIDTH, BASE_BALL_HEIGHT
                        };
                    state.ballVelocity = {0, -BASE_BALL_SPEED};
                    state.ballSticked = true;
                }
                else {
                    return true;
                }
            }
        }
        return false;
    }

    ~Ball() override = default;
};
