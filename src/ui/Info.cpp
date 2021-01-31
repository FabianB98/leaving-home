#include "Info.hpp"

#include <sstream>

namespace gui
{
	constexpr auto cellInfoMargin = 12.f;
	constexpr auto cellInfoWidth = 350.f;
	constexpr auto cellInfoHeight = 150.f;

	bool cellInfoOpen = false;
	ImVec2 cellInfoPos;
	bool updateCellInfoPos = false;

	game::world::Cell* selection;

	void openCellInfo(glm::vec2& mousePos, glm::vec2& frameSize, game::world::Cell* selected)
	{
		selection = selected;
		cellInfoOpen = true;
		updateCellInfoPos = true;

		float x = (mousePos.x + cellInfoMargin + cellInfoWidth <= frameSize.x) 
			? (mousePos.x + cellInfoMargin)
			: (mousePos.x - cellInfoWidth - cellInfoMargin);

		float y = (mousePos.y + cellInfoMargin + cellInfoHeight <= frameSize.y)
			? (mousePos.y + cellInfoMargin)
			: (mousePos.y - cellInfoHeight - cellInfoMargin);

		cellInfoPos = ImVec2(x, y);
	}

	void renderCellInfo(entt::registry& registry)
	{
		if (!cellInfoOpen) return;
		
		if (updateCellInfoPos) {
			updateCellInfoPos = false;
			ImGui::SetNextWindowPos(cellInfoPos);
			ImGui::SetNextWindowFocus();
		}

		ImGui::Begin("Info", &cellInfoOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(cellInfoWidth, cellInfoHeight));

		std::string cellType;
		switch (selection->getCellType()) {
			case game::world::CellType::GRASS: cellType = "Grass"; break;
			case game::world::CellType::STONE: cellType = "Stone"; break;
			case game::world::CellType::SNOW: cellType = "Snow"; break;
			case game::world::CellType::SAND: cellType = "Sand"; break;
		}

		game::world::CellContent* cellContent = selection->getContent();
		if (cellContent == nullptr)
		{
			ImGui::Text("Type: %s", cellType);
			ImGui::Text("Height: %i", (int)selection->getHeight());
			ImGui::Text("This cell is empty.");
		}
		else
		{
			//ImGui::Text("Content: %p", cellContent);
			ImGui::TextWrapped("%s", cellContent->getTypeName().c_str());
			ImGui::TextWrapped("%s", cellContent->getDescription().c_str());
			ImGui::TextUnformatted(cellContent->getInventoryContentsString().c_str());

			auto entity = cellContent->getEntity();
			auto* droneFactory = registry.try_get<game::world::DroneFactoryBuildingComponent>(entity);
			if (droneFactory != nullptr) {
				ImGui::TextWrapped("Drones to produce: %i", droneFactory->amountOfDronesToProduce);
				if (ImGui::Button("Produce"))
					droneFactory->amountOfDronesToProduce++;
			}
		}

		ImGui::End();
	}
}