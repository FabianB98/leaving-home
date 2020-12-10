#include "MovementInputSystem.hpp"

namespace game::systems
{
	const float TWO_TIMES_PI = 2.0f * M_PI;

	const float SMOOTHING_DISTANCE = 20.0f;
	const float SMOOTHING_WEIGHTS[9] = {
		1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f,
		1.0f / 8.0f , 1.0f / 4.0f, 1.0f / 8.0f ,
		1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f
	};

	using namespace rendering::components;
	using namespace game::components;

	bool lockMouse = false;
	bool unlockMouse = true;

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

	void updateHeightConstrainedMoveControllers(rendering::RenderingEngine* renderingEngine, entt::registry& registry, 
		double deltaTime, world::HeightGenerator& heightGenerator)
	{
		auto movementView = registry.view<EulerComponentwiseTransform, HeightConstrainedMoveController>();
		movementView.each([renderingEngine, &registry, deltaTime, &heightGenerator](auto entity, auto& transform, auto& controller) {
			int mouseButtonCode = controller.getMouseButtonCode();
			bool shouldMove = mouseButtonCode < 0 || renderingEngine->isMouseButtonPressed(mouseButtonCode);
			bool mouseLocked = renderingEngine->isMouseCursorLockedToCenter();

			if (shouldMove)
			{
				unlockMouse = false;
				if (mouseLocked)
				{
					// Determine the directions (forward and right) into which the entity should be moved if the mouse
					// was moved in the corresponding direction.
					float sinYaw = sin(transform.getYaw());
					float cosYaw = cos(transform.getYaw());
					glm::vec3 forward = glm::vec3(-sinYaw, 0, -cosYaw);
					glm::vec3 right = glm::vec3(cosYaw, 0, -sinYaw);

					glm::vec2 mouseDelta = renderingEngine->getMouseDelta();
					glm::vec2 movementDelta = mouseDelta * glm::vec2(deltaTime * controller.getMouseSensitivity());

					registry.patch<EulerComponentwiseTransform>(entity, [movementDelta, forward, right, &heightGenerator](auto& transform)
					{
						// Calculate the new position on the XZ plane.
						glm::vec3 oldPosition = transform.getTranslation();
						glm::vec3 newPosition = oldPosition - movementDelta.x * right + movementDelta.y * forward;

						// Average the heights around the new position with gaussian weights to determine the position along the Y axis.
						const float heights[9] = {
							heightGenerator.getHeight(newPosition.x - SMOOTHING_DISTANCE, newPosition.z - SMOOTHING_DISTANCE),
							heightGenerator.getHeight(newPosition.x - SMOOTHING_DISTANCE, newPosition.z                     ),
							heightGenerator.getHeight(newPosition.x - SMOOTHING_DISTANCE, newPosition.z + SMOOTHING_DISTANCE),
							heightGenerator.getHeight(newPosition.x                     , newPosition.z - SMOOTHING_DISTANCE),
							heightGenerator.getHeight(newPosition.x                     , newPosition.z                     ),
							heightGenerator.getHeight(newPosition.x                     , newPosition.z + SMOOTHING_DISTANCE),
							heightGenerator.getHeight(newPosition.x + SMOOTHING_DISTANCE, newPosition.z - SMOOTHING_DISTANCE),
							heightGenerator.getHeight(newPosition.x + SMOOTHING_DISTANCE, newPosition.z                     ),
							heightGenerator.getHeight(newPosition.x + SMOOTHING_DISTANCE, newPosition.z + SMOOTHING_DISTANCE)
						};

						newPosition.y = 0.0f;
						for (int i = 0; i < 9; i++)
						{
							newPosition.y += SMOOTHING_WEIGHTS[i] * (0.75f * heights[i] + 0.25f * heightGenerator.quantizeHeight(heights[i]));
						}

						// Ensure that the camera won't be under water.
						if (newPosition.y < game::world::WATER_HEIGHT)
							newPosition.y = game::world::WATER_HEIGHT;
						
						transform.setTranslation(newPosition);
					});
				}
				else
				{
					lockMouse = true;
				}
			}
		});
	}

	void updateAxisConstrainedMoveControllers(rendering::RenderingEngine* renderingEngine, entt::registry& registry, double deltaTime)
	{
		auto movementView = registry.view<EulerComponentwiseTransform, AxisConstrainedMoveController>();
		movementView.each([renderingEngine, &registry, deltaTime](auto entity, auto& transform, auto& controller) {
			float movementAlongAxis = deltaTime * controller.getMouseWheelSensitivity() * renderingEngine->getScrollDelta().y;

			if (movementAlongAxis != 0.0f)
			{
				// Calculate the new translation after the camera would be moved along the axis.
				glm::vec3 oldTranslation = transform.getTranslation();
				glm::vec3 deltaMovement = movementAlongAxis * controller.getAxis();
				glm::vec3 newTranslation = oldTranslation - deltaMovement;

				// Calculate the value along the axis (assuming the axis to be normalized) and constrain the value to be
				// within the defined range.
				float valueOnAxis = glm::dot(newTranslation, controller.getAxis());

				float distanceToMinimum = valueOnAxis - controller.getMinValue();
				float distanceToMaximum = controller.getMaxValue() - valueOnAxis;

				if (distanceToMinimum < 0.0f)
					newTranslation -= distanceToMinimum * controller.getAxis();
				else if (distanceToMaximum < 0.0f)
					newTranslation += distanceToMaximum * controller.getAxis();

				// Update the transform component.
				registry.patch<EulerComponentwiseTransform>(entity, [newTranslation](auto& transform)
				{
					transform.setTranslation(newTranslation);
				});
			}
		});
	}

	void updateFirstPersonRotateControllers(rendering::RenderingEngine* renderingEngine, entt::registry& registry,
		double deltaTime)
	{
		auto rotateView = registry.view<EulerComponentwiseTransform, FirstPersonRotateController>();
		rotateView.each([renderingEngine, &registry, deltaTime](auto entity, auto& transform, auto& controller) {
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

		
	}

	void updateMovementInputSystem(rendering::RenderingEngine* renderingEngine, double deltaTime, world::HeightGenerator& heightGenerator)
	{
		entt::registry& registry = renderingEngine->getRegistry();

		lockMouse = false;
		unlockMouse = true;

		updateFreeFlyingMoveControllers(renderingEngine, registry, deltaTime);
		updateHeightConstrainedMoveControllers(renderingEngine, registry, deltaTime, heightGenerator);
		updateAxisConstrainedMoveControllers(renderingEngine, registry, deltaTime);
		updateFirstPersonRotateControllers(renderingEngine, registry, deltaTime);

		if (lockMouse)
		{
			renderingEngine->lockMouseCursorToCenter(true);
		}
		else if (unlockMouse)
		{
			renderingEngine->lockMouseCursorToCenter(false);
		}
	}
}
