#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(AccelerationComponent)
	{
		ZT_COMPONENT_FIELDS(
			glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		);
	};
}