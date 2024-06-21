#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(VelocityComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(glm::vec3, velocity, glm::vec3(0.0f, 0.0f, 0.0f));
		);
	};
}