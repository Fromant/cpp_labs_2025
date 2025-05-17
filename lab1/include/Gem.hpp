#ifndef GEM_HPP
#define GEM_HPP
#include "SDL3/SDL_pixels.h"

struct Gem {
    explicit Gem(SDL_Color c) : color(c) {};
    Gem() = default;

    SDL_Color color{255, 255, 255, 255};
};


#endif //GEM_HPP
