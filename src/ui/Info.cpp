#include "Info.hpp"

namespace gui
{
	constexpr auto cellInfoMargin = 12.f;
	constexpr auto cellInfoWidth = 250.f;
	constexpr auto cellInfoHeight = 50.f;

	bool cellInfoOpen = false;
	ImVec2 cellInfoPos;
	bool updateCellInfoPos = false;

	void openCellInfo(glm::vec2& mousePos, glm::vec2& frameSize)
	{
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

		ImGui::Text("Very interesting information...");

		ImGui::End();
	}
}