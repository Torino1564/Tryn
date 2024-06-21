#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(PositionComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(glm::vec3, position);
		);
	};
}