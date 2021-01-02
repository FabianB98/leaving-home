#include "Info.hpp"

// TODO: This is just a temporary include for displaying the cell's content. Remove this later on.
#include "../game/world/Building.hpp"
#include <sstream>

namespace gui
{
	constexpr auto cellInfoMargin = 12.f;
	constexpr auto cellInfoWidth = 300.f; // TODO: Change this value back to 250.f when removing the temporary code for displaying the cell's content.
	constexpr auto cellInfoHeight = 85.f; // TODO: Change this value back to 80.f when removing the temporary code for displaying the cell's content.

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

	void renderCellInfo()
	{
		if (!cellInfoOpen) return;
		
		if (updateCellInfoPos) {
			updateCellInfoPos = false;
			ImGui::SetNextWindowPos(cellInfoPos);
			ImGui::SetNextWindowFocus();
		}

		ImGui::Begin("Info", &cellInfoOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(cellInfoWidth, cellInfoHeight));

		//selection->getCellType()

		std::string cellType;
		switch (selection->getCellType()) {
			case game::world::CellType::GRASS: cellType = "Grass"; break;
			case game::world::CellType::STONE: cellType = "Stone"; break;
			case game::world::CellType::SNOW: cellType = "Snow"; break;
			case game::world::CellType::SAND: cellType = "Sand"; break;
		}

		ImGui::Text("Type: %s", cellType);
		ImGui::Text("Height: %i", (int) selection->getHeight());

		// TODO: This is just temporary code for showing the cell's content. Remove this (and the temporary include at the top of this file) later on.
		game::world::CellContent* cellContent = selection->getContent();
		std::string cellContentText = "Unknown";
		if (cellContent == nullptr)
			cellContentText = "Empty";
		else if (dynamic_cast<game::world::Tree*>(cellContent))
			cellContentText = "Tree";
		else if (dynamic_cast<game::world::TestBuilding*>(cellContent))
			cellContentText = "TestBuilding";
		ImGui::Text("Content: %s %p", cellContentText, cellContent);

		ImGui::End();
	}
}