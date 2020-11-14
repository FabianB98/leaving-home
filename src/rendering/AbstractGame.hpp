#pragma once

namespace rendering
{
	class AbstractGame
	{
	private:

	public:
		AbstractGame() {};
		~AbstractGame() {};

		virtual void init() = 0;

		virtual void input(double deltaTime) = 0;

		virtual void update(double deltaTime) = 0;

		virtual void render() = 0;

		virtual void cleanUp() = 0;
	};
}
