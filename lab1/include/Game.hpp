#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include "Block.hpp"
#include "Bonus.hpp"

class Game {
public:
    Game();
    ~Game();

    bool Initialize();
    void Run();
    void ActivateBonus(BonusType type);
    void ResetGame();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void UpdateBonuses(float deltaTime);
    void UpdatePaddle(float deltaTime);
    void Render();
    void CheckCollisions();
    void HandleBallBlockCollision(Block& block);
    void SpawnBonus(float x, float y, BonusType type);
    void HandleBlockHit(Block& block);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool needsReset;

    // Game objects
    SDL_FRect paddle;
    SDL_FRect ball;
    SDL_FPoint ballVelocity;

    std::vector<std::unique_ptr<Block>> blocks;
    std::vector<std::unique_ptr<Bonus>> activeBonuses;

    // Game state
    int score;
    int lives;
    bool stickyPaddle;
    bool safetyNetActive;
    bool ballSticked = false;
    float paddleSpeedMultiplier;
    float ballSpeedMultiplier;
    const float maxBallSpeed = 500; // Максимальная скорость мяча
    const float ballRadius = ball.w / 2.0f; // Радиус для точных расчетов
    Uint64 safetyNetExpireTime = 0;
    Uint64 stickyPaddleExpireTime = 0;
};