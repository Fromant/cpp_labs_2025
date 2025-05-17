#include "Game.hpp"

#include <iostream>
#include <random>
#include <bits/uniform_int_dist.h>

#include "Colors.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int GEM_WIDTH = 40;
constexpr int GEM_HEIGHT = 40;
constexpr int TOTAL_GEMS = SCREEN_WIDTH * SCREEN_HEIGHT / GEM_WIDTH / GEM_HEIGHT;
constexpr int LINE_LENGTH = SCREEN_WIDTH / GEM_WIDTH;


Game::Game() {
    field.reserve(TOTAL_GEMS);
}

bool Game::Initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Создание окна
    window = SDL_CreateWindow("Gems",
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Создание рендерера
    renderer = SDL_CreateRenderer(window,
                                  nullptr);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    InitField();
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

    return true;
}

template <int start, int end>
int getRandomInt() {
    static std::random_device rd; // Seed for random number engine
    static std::mt19937 gen(rd()); // Mersenne Twister engine

    static std::uniform_int_distribution<int> dist(start, end);
    return dist(gen);
}

void Game::InitField() {
    for (int i = 0; i < TOTAL_GEMS; i++) {
        Gem gem;

        gem.color = PALETTE[getRandomInt<0, PALETTE_SIZE>()];

        field.emplace_back(gem);
    }
}


void Game::Run() {
    while (!shouldExit) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            shouldExit = true;
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            HandleClick(e.button);
        }
    }
}

bool isNeighbours(size_t a, size_t b) {
    if (a - b == 1 || a - b == -1) return true;
    if (a - b == LINE_LENGTH || a - b == -LINE_LENGTH) return true;
    return false;
}

void Game::HandleClick(const SDL_MouseButtonEvent& e) {
    if (e.button != SDL_BUTTON_LEFT || !e.down) {
        selectedGem = -1;
        return;
    }

    const int x = e.x / GEM_WIDTH;
    const int y = e.y / GEM_HEIGHT;

    size_t clicked = x + y * LINE_LENGTH;
    if (selectedGem != -1 && isNeighbours(selectedGem, clicked)) {
        //change gems
        auto tmp = field[selectedGem];
        field[selectedGem] = field[clicked];
        field[clicked] = tmp;

        //unselect
        selectedGem = -1;
    }
    //otherwise, change selection to clicked
    else selectedGem = clicked;
}


void Game::Render() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < field.size(); i++) {
        int gemX = i % LINE_LENGTH;
        int gemY = i / LINE_LENGTH;

        SDL_FRect gem{
                static_cast<float>(gemX * GEM_WIDTH), static_cast<float>(gemY * GEM_HEIGHT),
                GEM_WIDTH, GEM_HEIGHT
            };

        const SDL_Color& color = field[i].color;

        SDL_SetRenderDrawColor(renderer,
                               color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &gem);
    }

    //outline selected gem
    if (selectedGem != -1) {
        size_t gemX = selectedGem % LINE_LENGTH;
        size_t gemY = selectedGem / LINE_LENGTH;
        SDL_FRect gem{
                static_cast<float>(gemX * GEM_WIDTH), static_cast<float>(gemY * GEM_HEIGHT),
                GEM_WIDTH, GEM_HEIGHT
            };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 96);
        SDL_RenderFillRect(renderer, &gem);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &gem);
    }

    SDL_RenderPresent(renderer);
}

void Game::CheckGems() {}

void Game::Update() {}

void Game::SpawnBonuses() {}
