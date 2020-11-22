#include "TransformConversionSystem.hpp"

namespace rendering::systems
{
	void updateTransformConversion(entt::registry& registry)
	{
		registry.view<rendering::components::EulerComponentwiseTransform>().each([&registry](auto entity, auto& tf) {
			registry.emplace_or_replace<rendering::components::MatrixTransform>(entity, tf.toTransformationMatrix());
		});

		registry.view<rendering::components::QuaternionComponentwiseTransform>().each([&registry](auto entity, auto& tf) {
			registry.emplace_or_replace<rendering::components::MatrixTransform>(entity, tf.toTransformationMatrix());
		});
	}
}
