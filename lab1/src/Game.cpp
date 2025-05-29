#include "Game.hpp"
#include <random>
#include <algorithm>
#include <functional>
#include <iostream>

#include "bonuses/AccelerateBallBonus.hpp"
#include "bonuses/ExpandPaddleBonus.hpp"
#include "bonuses/RandomBallDirectionBonus.hpp"
#include "bonuses/SafetyNetBonus.hpp"
#include "bonuses/ShrinkPaddleBonus.hpp"
#include "bonuses/StickyPaddleBonus.hpp"

Game::Game() : window(nullptr), renderer(nullptr), isRunning(true), needsReset(false),
               ball{
                   SCREEN_WIDTH / 2 - BASE_BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BASE_BALL_WIDTH / 2, BASE_BALL_WIDTH,
                   BASE_BALL_HEIGHT
               }, paddle{
                   SCREEN_WIDTH / 2 - BASE_PADDLE_WIDTH / 2, SCREEN_HEIGHT - 2 * BASE_PADDLE_HEIGHT, BASE_PADDLE_WIDTH,
                   BASE_PADDLE_HEIGHT
               } {}

void Game::ResetGame() {
    // Reset game state
    state = DEFAULT_STATE;
    paddle = {
            SCREEN_WIDTH / 2 - BASE_PADDLE_WIDTH / 2, SCREEN_HEIGHT - 2 * BASE_PADDLE_HEIGHT, BASE_PADDLE_WIDTH,
            BASE_PADDLE_HEIGHT
        };
    ball = {
            SCREEN_WIDTH / 2 - BASE_BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BASE_BALL_WIDTH / 2, BASE_BALL_WIDTH,
            BASE_BALL_HEIGHT
        };
    // Rebuild blocks
    blocks.clear();
    const int rows = 4;
    const int cols = 10;
    const float blockWidth = 70;
    const float blockHeight = 20;
    const float spacing = 5;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> bonusDist(0, 5);

    const static std::array<std::function<Bonus*(int x, int y)>, 6> bonusFactories = {
            [](int x, int y) { return (Bonus*)new AccelerateBallBonus(x, y); },
            [](int x, int y) { return (Bonus*)new ExpandPaddleBonus(x, y); },
            [](int x, int y) { return (Bonus*)new RandomBallDirectionBonus(x, y); },
            [](int x, int y) { return (Bonus*)new SafetyNetBonus(x, y); },
            [](int x, int y) { return (Bonus*)new ShrinkPaddleBonus(x, y); },
            [](int x, int y) { return (Bonus*)new StickyPaddleBonus(x, y); }
        };

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float x = j * (blockWidth + spacing) + spacing;
            float y = i * (blockHeight + spacing) + 50;
            bool indestruct = (i == 0);
            bool accelerate = (i == 2);
            int health = indestruct ? 1 : (rows - i);
            Bonus* bonus = nullptr;

            if (!indestruct && (gen() % 4) == 0) {
                bonus = bonusFactories[bonusDist(gen)](x, y);
            }

            blocks.emplace_back(std::make_unique<Block>(
                x, y, blockWidth, blockHeight,
                health, indestruct, accelerate, bonus
            ));
        }
    }

    activeBonuses.clear();
}

void Game::CheckCollisions() {
    // Обработка границ экрана
    if (ball.getCollisionRect().x < 0) {
        ball.getCollisionRect().x = 0;
        state.ballVelocity.x = fabs(state.ballVelocity.x);
    }
    if (ball.getCollisionRect().x + ball.getCollisionRect().w > SCREEN_WIDTH) {
        ball.getCollisionRect().x = SCREEN_WIDTH - ball.getCollisionRect().w;
        state.ballVelocity.x = -fabs(state.ballVelocity.x);
    }
    if (ball.getCollisionRect().y < 0) {
        ball.getCollisionRect().y = 0;
        state.ballVelocity.y = fabs(state.ballVelocity.y);
    }

    if (ball.checkCollisionWith(paddle, state)) {
        //collision
        if (!state.stickyPaddle) {
            // Расчет точки удара [-0.5, 0.5]
            float hitPosition = (ball.getCollisionRect().x + ball.getCollisionRect().w / 2 - paddle.getCollisionRect().
                x) / paddle.getCollisionRect().w - 0.5f;
            float prevVelocity = sqrtf(
                state.ballVelocity.x * state.ballVelocity.x + state.ballVelocity.y * state.ballVelocity.y);
            state.ballVelocity.x = hitPosition * prevVelocity;
            state.ballVelocity.y = -sqrtf(prevVelocity * prevVelocity - state.ballVelocity.x * state.ballVelocity.x);
        }
        else {
            state.ballSticked = true;
            state.stickyPaddle = false;
        }
    }

    // Коллизия бонусов с кареткой
    for (auto& bonus : activeBonuses) {
        if (bonus->doRender) bonus->checkCollisionWith(paddle, state);
    }

    // Коллизия блоков с мячом
    for (auto& block : blocks) {
        block->checkCollisionWith(ball, state);
    }
}

void Game::Run() {
    Uint64 previousTicks = SDL_GetTicks();

    while (isRunning) {
        Uint64 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - previousTicks) / 1000.0f;
        previousTicks = currentTicks;

        ProcessInput();
        SDL_Delay(std::max(0.0f, 16 - deltaTime*1000));
        deltaTime = 0.016f;

        Update(deltaTime);
        Render();
    }
}

void Game::UpdatePaddle(const float deltaTime) {
    paddle.update(deltaTime, state);
}

void Game::UpdateBonuses(const float deltaTime) {
    // Движение бонусов вниз
    for (auto& bonus : activeBonuses) {
        bonus->update(deltaTime, state);
    }

    // Удаление неактивных бонусов
    activeBonuses.erase(
        std::remove_if(activeBonuses.begin(), activeBonuses.end(),
                       [](const auto& b) { return !b->isActive(); }),
        activeBonuses.end()
    );
}

void Game::Update(const float deltaTime) {
    // Обновление позиции мяча с учётом множителя скорости
    if (ball.update(deltaTime, state, paddle)) ResetGame();

    CheckCollisions();
    UpdateBonuses(deltaTime);
    UpdatePaddle(deltaTime);

    // Удаление разрушенных блоков и спавн бонусов
    auto bonusesRef = std::ref(activeBonuses);
    blocks.erase(
        std::remove_if(blocks.begin(), blocks.end(),
                       [bonusesRef](const std::unique_ptr<Block>& block) {
                           if (block->IsDestroyed()) {
                               //spawn bonus
                               if (block->innerBonus) {
                                   bonusesRef.get().emplace_back(block->innerBonus);
                                   block->innerBonus->enableRendering();
                               }
                               return true;
                           }
                           return false;
                       }),
        blocks.end()
    );
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    paddle.render(renderer);
    ball.render(renderer);

    // Отрисовка блоков
    for (const auto& block : blocks) {
        block->render(renderer);
    }

    // Отрисовка бонусов
    for (const auto& bonus : activeBonuses) {
        bonus->render(renderer);
    }

    // Отрисовка HUD
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    if (state.safetyNetActive) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderLine(renderer, 0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, SCREEN_HEIGHT - 20);
    }

    SDL_RenderPresent(renderer);
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                // Рестарт игры по R
                if (event.key.key == SDLK_R) {
                    ResetGame();
                }
                // Отпускание мяча при липкой каретке
                else if (state.ballSticked && event.key.key == SDLK_SPACE
                ) {
                    state.ballSticked = false;
                    state.ballVelocity = {0, -BASE_BALL_SPEED * state.ballSpeedMultiplier};
                }
                else if (event.key.key == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                // Отпускание мяча по клику мыши
                if (state.ballSticked) {
                    state.ballSticked = false;
                    state.ballVelocity = {0, -BASE_BALL_SPEED * state.ballSpeedMultiplier};
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                // Управление кареткой мышью
                paddle.getCollisionRect().x = event.motion.x - paddle.getCollisionRect().w / 2;
                break;
            default: continue;
        }
    }
}

bool Game::Initialize() {
    // Инициализация SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Создание окна
    window = SDL_CreateWindow("Arkanoid",
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

    // Инициализация игровых объектов
    ResetGame();

    return true;
}

Game::~Game() = default;
