#include "Main.hpp"

int main()
{
    game::Game* game = new game::Game();

    rendering::RenderingEngine* renderingEngine = new rendering::RenderingEngine(*game, "Leaving Home", 1920, 1080);
    renderingEngine->start();

    delete game;
    delete renderingEngine;
}
