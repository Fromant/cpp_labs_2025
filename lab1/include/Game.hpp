#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

#include "Ball.hpp"
#include "Block.hpp"
#include "Paddle.hpp"
#include "bonuses/Bonus.hpp"

class Game {
public:
    Game();
    ~Game();

    bool Initialize();
    void Run();
    void ResetGame();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void UpdateBonuses(float deltaTime);
    void UpdatePaddle(float deltaTime);
    void Render();
    void CheckCollisions();
    void HandleBallBlockCollision(Block& block);
    void SpawnBonus(float x, float y);
    void HandleBlockHit(Block& block);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool needsReset;

    // Game objects
    Paddle paddle;
    Ball ball;

    std::vector<std::unique_ptr<Block>> blocks;
    std::vector<std::unique_ptr<Bonus>> activeBonuses;


    GameState state{};
    static constexpr GameState DEFAULT_STATE{};
};
