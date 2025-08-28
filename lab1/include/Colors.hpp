#pragma once
#include "SDL3/SDL_pixels.h"

constexpr SDL_Color WHEAT{245, 222, 179, 255};
constexpr SDL_Color MAROON{128, 0, 0, 255};
constexpr SDL_Color ORANGE{255, 140, 0, 255};
constexpr SDL_Color OLIVE{128, 128, 0, 255};
constexpr SDL_Color LIGHT_PINK{255, 182, 193, 255};
constexpr SDL_Color SLATE_GRAY{112, 128, 144, 255};

constexpr SDL_Color PALETTE[] = {WHEAT, MAROON, ORANGE, OLIVE, LIGHT_PINK, SLATE_GRAY};
constexpr int PALETTE_SIZE = sizeof(PALETTE) / sizeof(SDL_Color);