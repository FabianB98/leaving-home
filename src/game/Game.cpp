#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/components/Relationship.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/Skybox.hpp"
#include "components/AxisConstrainedMoveController.hpp"
#include "components/FirstPersonRotateController.hpp"
#include "components/FreeFlyingMoveController.hpp"
#include "components/HeightConstrainedMoveController.hpp"
#include "systems/MovementInputSystem.hpp"
#include "DayNightCycle.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"
#include "world/Resource.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <time.h>

#include <chrono>

namespace game
{
	rendering::model::Mesh* tree;
	/*entt::entity tree;
	float rotation = 0;*/

	world::World* wrld;
	entt::entity sun;

	DayNightCycle daynight;
	rendering::Skybox* skybox;
	rendering::shading::Shader* simple;
	rendering::shading::Shader* waterShader;
	rendering::shading::Shader* terrainShader;

	rendering::textures::Texture2D* icon;

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		tree = new rendering::model::Mesh("tree");

		simple = new rendering::shading::Shader("simpleInstanced");
		waterShader = new rendering::shading::LightSupportingShader("waterInstanced", true);
		terrainShader = new rendering::shading::LightSupportingShader("terrainInstanced");

		skybox = new rendering::Skybox("skybox", "skybox");

		icon = new rendering::textures::Texture2D("edit");

		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		using namespace rendering::components;

		auto& registry = renderingEngine->getRegistry();
		auto& shading = registry.ctx<rendering::systems::MeshShading>();

		auto start = std::chrono::high_resolution_clock::now();

		wrld = new world::World(1337, registry, terrainShader, waterShader);
		int worldSize = 3;
		for (int column = -worldSize; column <= 0; column++)
			for (int row = -worldSize - column; row <= worldSize; row++)
				wrld->generateChunk(row, column);
		for (int column = 1; column <= worldSize; column++)
			for (int row = -worldSize; row <= worldSize - column; row++)
				wrld->generateChunk(row, column);
		

		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << "Generated " << wrld->getChunks().size() << " chunks in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms" << std::endl;

		entt::entity cameraBase = registry.create();
		registry.emplace<EulerComponentwiseTransform>(cameraBase, glm::vec3(0, 0, 0), 0, glm::radians(-40.0f), 0, glm::vec3(1.0f));
		registry.emplace<components::HeightConstrainedMoveController>(cameraBase, GLFW_MOUSE_BUTTON_RIGHT, 8.0f);
		registry.emplace<components::FirstPersonRotateController>(cameraBase, GLFW_MOUSE_BUTTON_MIDDLE, 0.2f, glm::radians(-90.0f), glm::radians(-10.0f));
		registry.emplace<rendering::components::Relationship>(cameraBase);

		entt::entity camera = renderingEngine->getMainCamera();
		registry.emplace<EulerComponentwiseTransform>(camera, glm::vec3(0, 0, 100), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<components::AxisConstrainedMoveController>(camera, glm::vec3(0, 0, 1), 1000.0f, 50.0f, 1000.0f);
		registry.emplace<rendering::components::Relationship>(camera);

		rendering::systems::relationship(registry, cameraBase, camera);

		sun = registry.create();
		registry.emplace<MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<DirectionalLight>(sun, glm::vec3(1), glm::vec3(2, 1, 1));
	}

	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		game::systems::updateMovementInputSystem(renderingEngine, deltaTime, wrld->getHeightGenerator());
	}

	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		auto& registry = renderingEngine->getRegistry();

		daynight.update(deltaTime);
		registry.replace<rendering::components::DirectionalLight>(sun, daynight.getSunColor(), daynight.getSunDirection());
	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		float time = daynight.getTime();

		skybox->use();
		skybox->getShader()->setUniformFloat("time", time);
		skybox->getShader()->setUniformVec3("sunColor", daynight.getSunColor());
		skybox->render(renderingEngine);


		//ImGui::ShowDemoWindow();

		ImGui::Begin("Test window");

		/*ImGui::SliderFloat("Red", &red, 0.f, 1.f);
		ImGui::SliderFloat("Green", &green, 0.f, 1.f);
		ImGui::SliderFloat("Blue", &blue, 0.f, 1.f);*/

		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::SliderFloat("Day-Night speed", &daynight.speed, -1.f, 1.f);

		ImGui::End();


        /*bool open = true;
        ShowExampleAppCustomRendering(&open);*/

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		bool open = true;
		float size = 160.f;
		float margin = 48.f;

		ImGui::Begin("Tools", &open, flags);
		ImGui::SetWindowSize("Tools", ImVec2(size * 5.1f, size * 1.1f));

		ImGui::SetWindowPos("Tools", ImVec2(margin, renderingEngine->getFrameBufferHeight() - margin - size));

		//ImGui::PushItemWidth(-ImGui::GetFontSize() * 10);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 p = ImGui::GetCursorScreenPos();
		auto colf = ImVec4(0.0f, 0.0f, 0.0f, 0.85f);
		const ImU32 col = ImColor(colf);

		ImVec2 center(p.x + size * .5f, p.y + size * .5f);
		draw_list->AddNgonFilled(center, size * .5f, col, 6);
		

		float cut = 16.f;
		ImVec2 p1(center.x + size * .5f + 20.f - 0.577f*cut, center.y + cut);
		ImVec2 p2(center.x + size * .25f + 20.f, center.y + 0.866f * 0.5f * size);
		
		ImVec2 points[] = { ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImVec2(p2.x + 200.f, p2.y), ImVec2(p1.x + 200.f, p1.y) };
		const ImU32 col2 = ImColor(ImVec4(0, 0, 0, 0.85f));
		draw_list->AddConvexPolyFilled(points, 4, col2);


		
		
		//ImGui::Button("Test");
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

		float iconSize = 26.f;

		float angle = -M_PI * 1.f / 6.f;
		for (int i = 0; i < 6; i++) {

			float pX = size * .5f + sin(angle) * 48.f;
			float pY = size * .5f + cos(angle) * 48.f;

			ImGui::SetCursorPos(ImVec2(pX - iconSize * .5f, pY - iconSize * .5f));
			ImGui::ImageButton((void*)icon->getId(), ImVec2(iconSize, iconSize), ImVec2(0.02, 0.02), ImVec2(0.98, 0.98), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));

			angle += M_PI / 3.f;
		}

		ImGui::PopStyleColor(3);

		ImGui::End();


	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete tree;
		delete wrld;
	}
}
