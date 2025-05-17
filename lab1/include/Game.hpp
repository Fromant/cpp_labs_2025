#pragma once
#include <vector>

#include "Gem.hpp"
#include "SDL3/SDL_render.h"

class Game {
public:
    Game();

    bool Initialize();
    void Run();
    ~Game() = default;

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<Gem> field;
    bool shouldExit = false;
    size_t selectedGem = -1;

    void InitField();
    bool canPlace(size_t i, SDL_Color color) const;

    void Render() const;
    void Update();
    void CheckTriplets();
    void SpawnBonuses();
    void ProcessInput();
    void HandleClick(const SDL_MouseButtonEvent& e);
};
