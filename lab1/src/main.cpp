#include "Game.hpp"

int main() {
    Game game;
    if(game.Initialize()) {
        game.Run();
    }
    return 0;
}