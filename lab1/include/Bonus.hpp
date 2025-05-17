#pragma once
#include "SDL3/SDL_pixels.h"

struct Bonus {
    enum BonusType {
        RECOLOR, BOMB, SIZE
    };

    size_t i;
    BonusType t;
    Bonus() = default;
    Bonus(int i, BonusType bonus) : i(i), t(bonus) {}

    static SDL_Color getColor(BonusType t) {
        if (t == RECOLOR) return SDL_Color{0, 0, 255, 255};
        if (t == BOMB) return SDL_Color{255, 0, 0, 255};
        return {0, 0, 0, 0};
    }
};
