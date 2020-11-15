#include "Game.hpp"

#include "../rendering/model/Mesh.hpp"

namespace game
{
	rendering::model::Mesh* mesh;

	void Game::init()
	{
		mesh = new rendering::model::Mesh("cube");
	}

	void Game::input(double deltaTime)
	{

	}

	void Game::update(double deltaTime)
	{

	}

	void Game::render()
	{

	}

	void Game::cleanUp()
	{
		delete mesh;
	}
}
