#include "ToolSelection.hpp"

namespace gui
{
	constexpr float tsMargin = 48.f;
	constexpr float tsSpacing = 16.f;
	constexpr float tsIconSize = 48.f;

	constexpr float bsIconWidth = 60.f;

	rendering::textures::Texture2D* iconBuild;
	rendering::textures::Texture2D* iconView;
	rendering::textures::Texture2D* iconRemove;

	rendering::textures::Texture2D* iconWoodcutterBuilding;
	rendering::textures::Texture2D* iconReforesterBuilding;
	//rendering::textures::Texture2D* iconTestBuilding;
	rendering::textures::Texture2D* iconFoodFactoryBuilding;
	rendering::textures::Texture2D* iconMineBuilding;
	rendering::textures::Texture2D* iconStorageBuilding;
	rendering::textures::Texture2D* iconDroneFactoryBuilding;
	rendering::textures::Texture2D* iconResidenceBuilding;

	void initToolSelection() 
	{
		iconBuild = new rendering::textures::Texture2D("build");
		iconView = new rendering::textures::Texture2D("view");
		iconRemove = new rendering::textures::Texture2D("remove");

		iconWoodcutterBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		iconReforesterBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		//iconTestBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		iconFoodFactoryBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		iconMineBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		iconStorageBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		iconDroneFactoryBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
		iconResidenceBuilding = new rendering::textures::Texture2D("buildingThumbnails/residence");
	}

	void renderToolSelectionWindow(Tool* selected, float frameHeight)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		bool open = true;

		constexpr int numTools = 3;
		float width = numTools * tsIconSize + (numTools + 1) * tsSpacing;
		float height = tsIconSize + 2 * tsSpacing + 14.f;

		ImGui::Begin("Tools", &open, flags);
		ImGui::SetWindowSize("Tools", ImVec2(width, height));
		ImGui::SetWindowPos("Tools", ImVec2(tsMargin, frameHeight - tsMargin - height));

		//ImGui::Text("Tools:");

		/*ImVec4 c1 = rgba(178, 190, 195, *selected == Tool::BUILD ? 1.f : 0.4f);
		ImVec4 c2 = rgba(85, 239, 196, *selected == Tool::VIEW ? 1.f : 0.4f);
		ImVec4 c3 = rgba(116, 185, 255, *selected == Tool::REMOVE ? 1.f : 0.4f);*/

		ImVec4 c1 = rgba(0, 139, 167, *selected == Tool::BUILD ? 1.f : 0.4f);
		ImVec4 c2 = rgba(200, 200, 200, *selected == Tool::VIEW ? 1.f : 0.4f);
		ImVec4 c3 = rgba(224, 80, 78, *selected == Tool::REMOVE ? 1.f : 0.4f);

		//ImGui::Button("Test");
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 1.f, 1.f, 0.75f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.25f));

		//ImGui::SetCursorPos(ImVec2(size * .5f + 4.f - iconSize * .5f, size * .5f + 4.f - iconSize * .5f));
		auto p = ImGui::GetCursorPos();
		if (ImGui::ImageButton((void*)iconBuild->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c1))
			*selected = Tool::BUILD;

		ImGui::SetCursorPos(ImVec2(p.x + tsIconSize + tsSpacing, p.y));
		if (ImGui::ImageButton((void*)iconView->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c2))
			*selected = Tool::VIEW;

		ImGui::SetCursorPos(ImVec2(p.x + 2 * (tsIconSize + tsSpacing), p.y));
		if (ImGui::ImageButton((void*)iconRemove->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c3))
			*selected = Tool::REMOVE;

		ImGui::SetCursorPos(ImVec2(p.x + 10.f, p.y + tsIconSize + tsSpacing));
		ImGui::Text("Build");

		ImGui::SetCursorPos(ImVec2(p.x + 10.f + (tsIconSize + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("View");

		ImGui::SetCursorPos(ImVec2(p.x + 12.f + 2 * (tsIconSize + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Clear");

		ImGui::PopStyleColor(3);

		ImGui::End();
	}

	void renderBuildingSelectionWindow(game::world::IBuilding** selectedBuilding, float frameHeight)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		bool open = true;

		constexpr int numTools = 3;
		float width = numTools * tsIconSize + (numTools + 1) * tsSpacing;
		float height = tsIconSize + 2 * tsSpacing + 14.f;

		ImGui::Begin("Building", &open, flags);
		ImGui::SetWindowPos("Building", ImVec2(2.f * tsMargin + width, frameHeight - tsMargin - height));
		constexpr int numBuildings = 7;
		width = numBuildings * bsIconWidth + (numBuildings + 1) * tsSpacing;
		ImGui::SetWindowSize("Building", ImVec2(width, height));

		ImVec4 c1 = rgba(255, 255, 255, *selectedBuilding == &game::world::WoodcutterBuilding::typeRepresentative ? 1.f : 0.4f);
		ImVec4 c2 = rgba(255, 255, 255, *selectedBuilding == &game::world::ReforesterBuilding::typeRepresentative ? 1.f : 0.4f);
		ImVec4 c3 = rgba(255, 255, 255, *selectedBuilding == &game::world::FoodFactoryBuilding::typeRepresentative ? 1.f : 0.4f);
		ImVec4 c4 = rgba(255, 255, 255, *selectedBuilding == &game::world::MineBuilding::typeRepresentative ? 1.f : 0.4f);
		ImVec4 c5 = rgba(255, 255, 255, *selectedBuilding == &game::world::StorageBuilding::typeRepresentative ? 1.f : 0.4f);
		ImVec4 c6 = rgba(255, 255, 255, *selectedBuilding == &game::world::DroneFactoryBuilding::typeRepresentative ? 1.f : 0.4f);
		ImVec4 c7 = rgba(255, 255, 255, *selectedBuilding == &game::world::ResidenceBuilding::typeRepresentative ? 1.f : 0.4f);

		//ImGui::Button("Test");
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 1.f, 1.f, 0.75f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.25f));

		//ImGui::SetCursorPos(ImVec2(size * .5f + 4.f - iconSize * .5f, size * .5f + 4.f - iconSize * .5f));
		auto p = ImGui::GetCursorPos();
		if (ImGui::ImageButton((void*)iconWoodcutterBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c1))
			*selectedBuilding = &game::world::WoodcutterBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + bsIconWidth + tsSpacing, p.y));
		if (ImGui::ImageButton((void*)iconReforesterBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c2))
			*selectedBuilding = &game::world::ReforesterBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + 2.f * (bsIconWidth + tsSpacing), p.y));
		if (ImGui::ImageButton((void*)iconFoodFactoryBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c3))
			*selectedBuilding = &game::world::FoodFactoryBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + 3.f * (bsIconWidth + tsSpacing), p.y));
		if (ImGui::ImageButton((void*)iconMineBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c4))
			*selectedBuilding = &game::world::MineBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + 4.f * (bsIconWidth + tsSpacing), p.y));
		if (ImGui::ImageButton((void*)iconStorageBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c5))
			*selectedBuilding = &game::world::StorageBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + 5.f * (bsIconWidth + tsSpacing), p.y));
		if (ImGui::ImageButton((void*)iconDroneFactoryBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c6))
			*selectedBuilding = &game::world::DroneFactoryBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + 6.f * (bsIconWidth + tsSpacing), p.y));
		if (ImGui::ImageButton((void*)iconResidenceBuilding->getId(), ImVec2(tsIconSize, tsIconSize),
			ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), c7))
			*selectedBuilding = &game::world::ResidenceBuilding::typeRepresentative;

		ImGui::SetCursorPos(ImVec2(p.x + 0.f, p.y + tsIconSize + tsSpacing));
		ImGui::Text("Woodcutter");

		ImGui::SetCursorPos(ImVec2(p.x + 0.f + 1.f * (bsIconWidth + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Reforester");

		ImGui::SetCursorPos(ImVec2(p.x + 0.f + 2.f * (bsIconWidth + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Food Fact.");

		ImGui::SetCursorPos(ImVec2(p.x + 15.f + 3.f * (bsIconWidth + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Mine");

		ImGui::SetCursorPos(ImVec2(p.x + 5.f + 4.f * (bsIconWidth + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Storage");

		ImGui::SetCursorPos(ImVec2(p.x - 5.f + 5.f * (bsIconWidth + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Drone Hub");

		ImGui::SetCursorPos(ImVec2(p.x + 0.f + 6.f * (bsIconWidth + tsSpacing), p.y + tsIconSize + tsSpacing));
		ImGui::Text("Residence");

		ImGui::PopStyleColor(3);

		ImGui::End();
	}

	void renderToolSelection(Tool* selected, game::world::IBuilding** selectedBuilding, float frameHeight)
	{
		renderToolSelectionWindow(selected, frameHeight);

		// building selection
		if (*selected == Tool::BUILD)
			renderBuildingSelectionWindow(selectedBuilding, frameHeight);
	}

	void cleanUpToolSelection()
	{
		delete iconBuild;
		delete iconView;
		delete iconRemove;
	}
}