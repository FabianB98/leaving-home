#pragma once

namespace rendering 
{
	class AbstractGame
	{
	private:

	public:
		AbstractGame();
		~AbstractGame();

		void init();

		void input(double deltaTime);

		void update(double deltaTime);

		void render();

		void cleanUp();
	}
}
