#pragma once
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const = 0;
    virtual void Update() {}
};