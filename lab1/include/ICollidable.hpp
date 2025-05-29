#pragma once

#include "SDL3/SDL_rect.h"

struct GameState;

class ICollidable {
protected:
    virtual void hit(GameState& gameState, SDL_FRect& collisionResult) = 0;

public:
    virtual SDL_FRect& getCollisionRect() = 0;

    virtual bool checkCollisionWith(ICollidable& other, GameState& state) {
        SDL_FRect intersection;
        if (SDL_GetRectIntersectionFloat(&other.getCollisionRect(), &getCollisionRect(), &intersection)) {
            //collision
            hit(state, intersection);
            other.hit(state, intersection);
            return true;
        }
        return false;
    }

    virtual ~ICollidable() = default;
};
