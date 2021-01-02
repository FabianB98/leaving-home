#include "PickingChunkSelection.hpp"

namespace game
{
	void selectChunks(entt::registry& registry, rendering::RenderingEngine* renderingEngine, world::World* wrld)
	{
		auto& camera = registry.get<rendering::components::Camera>(renderingEngine->getMainCamera());
		auto camPos = camera.getPosition();
		
		// transform mouse position to screen space xy coordinates
		glm::vec2 mousePos = renderingEngine->getMousePosition();
		glm::vec2 screenSize = renderingEngine->getFramebufferSize();
		mousePos.y = screenSize.y - mousePos.y;
		glm::vec2 screenPos = 2.f * mousePos / screenSize - glm::vec2(1.f, 1.f);

		// calculate view direction
		auto vpi = glm::inverse(camera.getViewProjectionMatrix());
		auto viewDir = glm::normalize(glm::vec3(vpi * glm::vec4(screenPos.x, screenPos.y, -1.f, 1.f)) - camPos);

		auto& picking = registry.ctx<rendering::systems::Picking>();
		picking.enabled.clear();

		for (auto& pair : wrld->getChunks()) {
			auto& chunkPos = pair.first;
			auto* chunk = pair.second;

			// get view line (only xz components)
			auto a = glm::vec2(camPos.x, camPos.z);
			auto b = glm::vec2(viewDir.x, viewDir.z);
			auto p = chunk->getCenterPos();

			// line-point-distance
			auto l = p - a;
			auto dist = glm::length(l.x * b.y - b.x * l.y) / glm::length(b);

			// check if chunk is near view line (and y coordinate >= 0)
			auto radius = 1.1f * 2.f * world::CHUNK_SIZE * world::CELL_SIZE;
			auto height = (camPos + (glm::length(l) - radius) * viewDir).y;
			if (dist <= radius && height >= 0)
				picking.enabled.insert(chunk->getLandscapeMesh());
		}
	}
}