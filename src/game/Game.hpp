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

		void input(rendering::RenderingEngine* renderingEngine, double deltaTime);

		void update(rendering::RenderingEngine* renderingEngine, double deltaTime);

		void render(rendering::RenderingEngine* renderingEngine);

		void cleanUp(rendering::RenderingEngine* renderingEngine);
	};
}
