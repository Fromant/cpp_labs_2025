#include "Bonus.hpp"
#include "Game.hpp"
#include "Gem.hpp"
#include "Colors.hpp"
#include <random>
#include <algorithm>

namespace {
    int getRandomInt(int start, int end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(start, end - 1);
        return dist(gen);
    }

    bool isNeighbours(size_t a, size_t b) {
        if (a == b) return false;
        if (a > b) std::swap(a, b);
        if (b - a == 1 && a % LINE_LENGTH != LINE_LENGTH-1) return true;
        if (b - a == LINE_LENGTH) return true;
        return false;
    }
}

void RecolorBonus::Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_FRect bonusRect = {
        rect.x + rect.w * 3/8,
        rect.y + rect.h * 3/8,
        rect.w / 4,
        rect.h / 4
    };
    SDL_RenderFillRect(renderer, &bonusRect);
}

void RecolorBonus::Activate(Game* game, size_t index) {
    auto& field = game->GetField();
    auto gem = std::dynamic_pointer_cast<Gem>(field[index]);
    if (!gem) return;

    auto src_color = gem->color;
    gem->color = PALETTE[getRandomInt(0, PALETTE_SIZE)];

    constexpr int RECOLOR_RADIUS = 2;
    int x = index % LINE_LENGTH;
    int y = index / LINE_LENGTH;

    int x1 = getRandomInt(std::max(0, x - RECOLOR_RADIUS), std::min(x + RECOLOR_RADIUS, LINE_LENGTH));
    int y1 = getRandomInt(std::max(0, y - RECOLOR_RADIUS), std::min(y + RECOLOR_RADIUS, ROWS));

    size_t i1 = x1 + y1 * LINE_LENGTH;

    while (isNeighbours(index, i1)) {
        x1 = getRandomInt(std::max(0, x - RECOLOR_RADIUS), std::min(x + RECOLOR_RADIUS, LINE_LENGTH));
        y1 = getRandomInt(std::max(0, y - RECOLOR_RADIUS), std::min(y + RECOLOR_RADIUS, ROWS));
        i1 = x1 + y1 * LINE_LENGTH;
    }

    if (auto other = std::dynamic_pointer_cast<Gem>(field[i1])) {
        other->color = src_color;
    }
}

void BombBonus::Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect bonusRect = {
        rect.x + rect.w * 3/8,
        rect.y + rect.h * 3/8,
        rect.w / 4,
        rect.h / 4
    };
    SDL_RenderFillRect(renderer, &bonusRect);
}

void BombBonus::Activate(Game* game, size_t index) {
    auto& field = game->GetField();
    for (int i = 0; i < 4; i++) {
        int x = getRandomInt(0, LINE_LENGTH);
        int y = getRandomInt(0, ROWS);
        size_t j = x + y * LINE_LENGTH;

        if (j < field.size()) {
            for (int row = y; row > 0; row--) {
                size_t current = x + row * LINE_LENGTH;
                size_t above = x + (row-1) * LINE_LENGTH;
                field[current] = field[above];
            }
            field[x] = std::make_shared<Gem>(PALETTE[getRandomInt(0, PALETTE_SIZE)]);
        }
    }
}