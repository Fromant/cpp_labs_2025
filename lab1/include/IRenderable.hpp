#pragma once

class IRenderable {
public:
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual ~IRenderable() = default;
};
