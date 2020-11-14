#pragma once

#include "../rendering/AbstractGame.hpp"

namespace game 
{
	class Game : public rendering::AbstractGame 
	{
	public:
		Game() {};
		~Game() {};

		void init();

		void input(double deltaTime);

		void update(double deltaTime);

		void render();

		void cleanUp();
	};
}
