#include "MovementInputSystem.hpp"

namespace game::systems
{
	const float TWO_TIMES_PI = 2.0f * M_PI;

	using namespace rendering::components;
	using namespace game::components;

	void updateFreeFlyingMoveControllers(rendering::RenderingEngine* renderingEngine, entt::registry& registry,
		double deltaTime)
	{
		auto movementView = registry.view<EulerComponentwiseTransform, FreeFlyingMoveController>();
		movementView.each([renderingEngine, &registry, deltaTime](auto entity, auto& transform, auto& controller) {
			// Determine the directions (forward, right and up) into which the entity should be moved if the
			// corresponding keys are pressed.
			float sinYaw = sin(transform.getYaw());
			float cosYaw = cos(transform.getYaw());
			glm::vec3 forward = glm::vec3(-sinYaw, 0, -cosYaw);
			glm::vec3 right = glm::vec3(cosYaw, 0, -sinYaw);
			glm::vec3 up = glm::vec3(0, 1, 0);

			glm::vec3 movementDirection = glm::vec3(0, 0, 0);

			// Determine movement along forward facing axis.
			if (renderingEngine->isKeyPressed(controller.getForwardKey()))
				movementDirection += forward;
			if (renderingEngine->isKeyPressed(controller.getBackwardKey()))
				movementDirection -= forward;

			// Determine movement along sideways facing axis.
			if (renderingEngine->isKeyPressed(controller.getRightKey()))
				movementDirection += right;
			if (renderingEngine->isKeyPressed(controller.getLeftKey()))
				movementDirection -= right;

			// Determine movement along upwards facing axis.
			if (renderingEngine->isKeyPressed(controller.getUpKey()))
				movementDirection += up;
			if (renderingEngine->isKeyPressed(controller.getDownKey()))
				movementDirection -= up;

			// Update position of the transform component according to the movement.
			if (movementDirection.x != 0.0f || movementDirection.y != 0.0f || movementDirection.z != 0.0f)
			{
				float deltaMovement = deltaTime * controller.getMovementSpeed();
				movementDirection = glm::normalize(movementDirection) * glm::vec3(deltaMovement);

				registry.patch<EulerComponentwiseTransform>(entity, [movementDirection](auto& transform)
				{
					transform.setTranslation(transform.getTranslation() + movementDirection);
				});
			}
		});
	}

	void updateFirstPersonRotateControllers(rendering::RenderingEngine* renderingEngine, entt::registry& registry,
		double deltaTime)
	{
		bool lockMouse = false;
		bool unlockMouse = true;

		auto rotateView = registry.view<EulerComponentwiseTransform, FirstPersonRotateController>();
		rotateView.each([renderingEngine, &registry, deltaTime, &lockMouse, &unlockMouse](auto entity, auto& transform, auto& controller) {
			int mouseButtonCode = controller.getMouseButtonCode();
			bool shouldRotate = mouseButtonCode < 0 || renderingEngine->isMouseButtonPressed(mouseButtonCode);
			bool mouseLocked = renderingEngine->isMouseCursorLockedToCenter();

			if (shouldRotate)
			{
				unlockMouse = false;
				if (mouseLocked)
				{
					float minPitch = controller.getMinPitch();
					float maxPitch = controller.getMaxPitch();

					glm::vec2 mouseDelta = renderingEngine->getMouseDelta();
					glm::vec2 movementDelta = mouseDelta * glm::vec2(deltaTime * controller.getMouseSensitivity());

					registry.patch<EulerComponentwiseTransform>(entity, [movementDelta, minPitch, maxPitch](auto& transform)
					{
						transform.setYaw(std::fmod(transform.getYaw() - movementDelta.x, TWO_TIMES_PI));
						transform.setPitch(std::clamp(transform.getPitch() - movementDelta.y, minPitch, maxPitch));
					});
				}
				else
				{
					lockMouse = true;
				}
			}
		});

		if (lockMouse)
		{
			renderingEngine->lockMouseCursorToCenter(true);
		}
		else if (unlockMouse)
		{
			renderingEngine->lockMouseCursorToCenter(false);
		}
	}

	void updateMovementInputSystem(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		entt::registry& registry = renderingEngine->getRegistry();

		updateFreeFlyingMoveControllers(renderingEngine, registry, deltaTime);
		updateFirstPersonRotateControllers(renderingEngine, registry, deltaTime);
	}
}
