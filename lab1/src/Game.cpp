#include "Game.hpp"

#include <iostream>
#include <random>
#include <bits/uniform_int_dist.h>

#include "Colors.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int GEM_WIDTH = 40;
constexpr int GEM_HEIGHT = 40;
constexpr int TOTAL_GEMS = SCREEN_WIDTH * SCREEN_HEIGHT / GEM_WIDTH / GEM_HEIGHT;
constexpr int LINE_LENGTH = SCREEN_WIDTH / GEM_WIDTH;
constexpr int ROWS = SCREEN_HEIGHT / GEM_HEIGHT;


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

int getRandomInt(int start, int end) {
    static std::random_device rd; // Seed for random number engine
    static std::mt19937 gen(rd()); // Mersenne Twister engine

    std::uniform_int_distribution<int> dist(start, end - 1);
    return dist(gen);
}

bool operator==(const SDL_Color& a, const SDL_Color& b) {
    if (a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a) return false;
    return true;
}

bool Game::canPlace(size_t i, SDL_Color src) const {
    if (i >= 2 * LINE_LENGTH)
        if (field[i - LINE_LENGTH].color == src && field[i - 2 * LINE_LENGTH].color == src)
            return false;

    if (i % LINE_LENGTH >= 2)
        if (field[i - 1].color == src && field[i - 2].color == src)
            return false;
    return true;
}


void Game::InitField() {
    for (int i = 0; i < TOTAL_GEMS; i++) {
        Gem gem;
        auto color = PALETTE[getRandomInt(0, PALETTE_SIZE)];
        while (!canPlace(i, color)) {
            color = PALETTE[getRandomInt(0, PALETTE_SIZE)];
        }

        gem.color = color;
        field.emplace_back(gem);
    }
}


void Game::Run() {
    while (!shouldExit) {
        ProcessInput();
        Update();
        Render();
        SDL_Delay(100);
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
    //otherwise, change selection to clicked gem
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

    //render bonuses
    for (const auto& bonus : bonuses) {
        int x = bonus.i % LINE_LENGTH;
        int y = bonus.i / LINE_LENGTH;
        const SDL_Color& color = Bonus::getColor(bonus.t);

        SDL_SetRenderDrawColor(renderer,
                               color.r, color.g, color.b, color.a);

        SDL_FRect f{
                float(x * GEM_WIDTH + GEM_WIDTH / 8 * 3),
                float(y * GEM_HEIGHT + GEM_HEIGHT / 8 * 3),
                GEM_WIDTH / 4, GEM_HEIGHT / 4
            };

        SDL_RenderFillRect(renderer, &f);
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

bool Game::CheckTriplets() {
    bool located = false;
    for (int i = 2; i < TOTAL_GEMS; i++) {
        const auto& src = field[i].color;

        if (i >= 2 * LINE_LENGTH &&
            field[i - LINE_LENGTH].color == src && field[i - 2 * LINE_LENGTH].color == src) {
            //located vertical triplet
            located = true;

            int additional = 1;
            while (i + additional * LINE_LENGTH < TOTAL_GEMS && field[i + additional * LINE_LENGTH].color == src)
                additional++;
            additional--;

            //move all gems in vertical downwards
            int j = i + additional * LINE_LENGTH;
            for (int dj = 0; dj < 3 + additional; dj++) {
                SpawnBonus(j - dj * LINE_LENGTH);
            }
            for (; j > (3 + additional) * LINE_LENGTH; j -= LINE_LENGTH) {
                field[j] = field[j - (3 + additional) * LINE_LENGTH];
            }

            for (; j > 0; j -= LINE_LENGTH) {
                //generate new gems
                Gem gem;
                auto color = PALETTE[getRandomInt(0, PALETTE_SIZE)];

                gem.color = color;
                field[j] = gem;
            }
            break;
        }

        if (i % LINE_LENGTH >= 2 &&
            field[i - 1].color == src && field[i - 2].color == src) {
            //located horizontal triplet
            located = true;

            int additional = 1;
            int endLineIndex = (i / LINE_LENGTH + 1) * LINE_LENGTH;
            while (i + additional < endLineIndex && field[i + additional].color == src)
                additional++;

            additional--;

            //move all gems from upwards
            for (int x = i + additional; x > i - 3; x--) {
                int j = x;
                SpawnBonus(j);
                for (; j > LINE_LENGTH; j -= LINE_LENGTH) {
                    field[j] = field[j - LINE_LENGTH];
                }
                Gem gem;
                auto color = PALETTE[getRandomInt(0, PALETTE_SIZE)];

                gem.color = color;
                field[j] = gem;
            }
            break;
        }
    }
    return located;
}

void Game::Update() {
    RunBonuses();
    CheckTriplets();
}

void Game::RunBonuses() {
    if (bonuses.empty()) return;
    //run only one bonus at a time
    const auto& bonus = bonuses.back();

    if (bonus.t == Bonus::BOMB) {
        int x = bonus.i % LINE_LENGTH;
        int y = bonus.i / LINE_LENGTH;

        for (int i = 0; i < 4; i++) {
            //destroy gem at (x,y)
            int j = x + y * LINE_LENGTH;
            for (; j > LINE_LENGTH; j -= LINE_LENGTH) {
                field[j] = field[j - LINE_LENGTH];
            }
            Gem n;
            n.color = PALETTE[getRandomInt(0,PALETTE_SIZE)];
            field[j]=n;


            //generate new x,y
            x = getRandomInt(0, LINE_LENGTH);
            y = getRandomInt(0, ROWS);
        }
    }
    else if (bonus.t == Bonus::RECOLOR) {}

    bonuses.pop_back();
}


void Game::SpawnBonus(size_t destroyed) {
    constexpr float SPAWN_CHANCE = 0.5;
    constexpr int SPAWN_RADIUS = 3;
    if (static_cast<float>(getRandomInt(0, 101)) / 100 > SPAWN_CHANCE) {
        //do spawn

        int x = destroyed % LINE_LENGTH;
        int y = destroyed / LINE_LENGTH;

        int xNew = getRandomInt(std::max(0, x - SPAWN_RADIUS),
                                std::min(LINE_LENGTH, x + SPAWN_RADIUS));
        int yNew = getRandomInt(std::max(0, y - SPAWN_RADIUS),
                                std::min(ROWS, y + SPAWN_RADIUS));

        bonuses.emplace_back(
            xNew + yNew * LINE_LENGTH,
            static_cast<Bonus::BonusType>(getRandomInt(0, Bonus::BonusType::SIZE))
        );
    }
}
