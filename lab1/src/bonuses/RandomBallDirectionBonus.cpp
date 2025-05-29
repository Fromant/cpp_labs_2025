#include "bonuses/RandomBallDirectionBonus.hpp"

#include <random>

void RandomBallDirectionBonus::render(SDL_Renderer* renderer) {
    if (!doRender) return;
    SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
}

void RandomBallDirectionBonus::activate(GameState& game) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angleDist(0.0f, 2 * M_PI);
    float angle = angleDist(gen);
    float xMult = std::cos(angle);
    float yMult = std::sin(angle);
    game.ballVelocity.x *= xMult;
    game.ballVelocity.y *= yMult;
    deactivate(game);
}

void RandomBallDirectionBonus::deactivate(GameState& game) {
    active = false;
    doRender = false;
}
