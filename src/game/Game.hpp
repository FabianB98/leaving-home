#pragma once

#include "../rendering/RenderingEngine.hpp"

namespace game 
{
	class Game : public rendering::AbstractGame 
	{
	public:
		Game() {};
		~Game() {};

		void init(rendering::RenderingEngine* renderingEngine);

		void input(double deltaTime);

		void update(double deltaTime);

		void render(rendering::RenderingEngine* renderingEngine);

		void cleanUp();
	};
}
