#include "Main.hpp"

int main()
{
    game::Game* game = new game::Game();

    rendering::RenderingEngine* renderingEngine = new rendering::RenderingEngine(*game, "Leaving Home", 1080, 720);
    renderingEngine->start();

    delete game;
    delete renderingEngine;
}
