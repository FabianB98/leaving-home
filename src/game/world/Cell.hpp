#pragma once

#include <stdlib.h>
#include <vector>

#include <glm/glm.hpp>

#define CELL_SIZE 1

namespace game::world
{
	class Cell
	{
	public:
		Cell(glm::vec2 _posRelativeToChunkCenter) : posRelativeToChunkCenter(_posRelativeToChunkCenter) {};

		Cell(glm::vec2 _posRelativeToChunkCenter, std::vector<Cell*> _neighbors)
			: posRelativeToChunkCenter(_posRelativeToChunkCenter), neighbors(_neighbors) {};

		glm::vec2 getPosRelativeToChunkCenter()
		{
			return posRelativeToChunkCenter;
		}

		std::vector<Cell*> getNeighbors()
		{
			return neighbors;
		}

	private:
		glm::vec2 posRelativeToChunkCenter;
		std::vector<Cell*> neighbors;
	};
}
