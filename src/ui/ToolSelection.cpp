#include "ToolSelection.hpp"

namespace gui
{
	constexpr float tsMargin = 48.f;
	constexpr float tsSpacing = 16.f;
	constexpr float tsIconSize = 48.f;

	rendering::textures::Texture2D* iconBuild;
	rendering::textures::Texture2D* iconView;
	rendering::textures::Texture2D* iconRemove;

	void initToolSelection() 
	{
		iconBuild = new rendering::textures::Texture2D("build");
		iconView = new rendering::textures::Texture2D("view");
		iconRemove = new rendering::textures::Texture2D("remove");
	}

	void renderToolSelection(Tool* selected, float frameHeight)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		bool open = true;
		

		float width = 3 * tsIconSize + 4 * tsSpacing;
		float height = tsIconSize + 2 * tsSpacing + 14.f;

		ImGui::Begin("Tools", &open, flags);
		ImGui::SetWindowSize("Tools", ImVec2(width, height));
		ImGui::SetWindowPos("Tools", ImVec2(tsMargin, frameHeight - tsMargin - height));

		//ImGui::Text("Tools:");

		ImVec4 c1 = rgba(178, 190, 195, *selected == Tool::BUILD ? 1.f : 0.4f);
		ImVec4 c2 = rgba(85, 239, 196, *selected == Tool::VIEW ? 1.f : 0.4f);
		ImVec4 c3 = rgba(116, 185, 255, *selected == Tool::REMOVE ? 1.f : 0.4f);

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

	void cleanUpToolSelection()
	{
		delete iconBuild;
		delete iconView;
		delete iconRemove;
	}
}