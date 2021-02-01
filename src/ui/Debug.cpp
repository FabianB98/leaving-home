#include "Debug.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace game::systems
{
	extern float droneMovementSpeedMultiplier;
}

namespace gui
{
	void renderDebugWindow(game::DayNightCycle& daynight, CameraType* camera)
	{
		ImGui::Begin("Debug");

		ImGui::Text("Day-Night Cycle Options:");

		//ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(rgba(97, 97, 97, 0.6f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(rgba(97, 97, 97, 0.7f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(rgba(97, 97, 97, 8.f)));
		if (ImGui::Button("Midnight"))
			daynight.setTime(-M_PI);
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(rgba(251, 192, 45, 0.6f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(rgba(251, 192, 45, 0.7f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(rgba(251, 192, 45, 0.8f)));
		if (ImGui::Button("Sunrise"))
			daynight.setTime(-M_PI * .6f);
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(rgba(33, 150, 243, 0.6f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(rgba(33, 150, 243, 0.7f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(rgba(33, 150, 243, 0.8f)));
		if (ImGui::Button("Noon"))
			daynight.setTime(0.f);
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(rgba(233, 30, 99, 0.6f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(rgba(233, 30, 99, 0.7f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(rgba(233, 30, 99, 0.8f)));
		if (ImGui::Button("Sunset"))
			daynight.setTime(M_PI * .4f);
		ImGui::PopStyleColor(3);

		//ImGui::SliderFloat("Speed", &daynight.speed, 0.f, 1.f);
		int speed = daynight.speed / 0.01f;
		ImGui::SliderInt("Speed", &speed, 0, 50, "%dx");
		daynight.speed = speed * 0.01f;

		ImGui::Separator();

		ImGui::Text("Drone Settings");
		int droneSpeed = (int) game::systems::droneMovementSpeedMultiplier;
		ImGui::SliderInt("Drone Speed", &droneSpeed, 0, 10, "%dx");
		game::systems::droneMovementSpeedMultiplier = (float) droneSpeed;

		//std::cout << game::systems::droneMovementSpeedMultiplier << std::endl;


		ImGui::Separator();

		ImGui::Text("Camera Options");

		int selected = *camera == CameraType::DEFAULT ? 0 : 1;
		ImGui::RadioButton("Default", &selected, 0); ImGui::SameLine();
		ImGui::RadioButton("Free flight", &selected, 1);
		*camera = selected == 0 ? CameraType::DEFAULT : CameraType::FREE;

		ImGui::End();
	}
}