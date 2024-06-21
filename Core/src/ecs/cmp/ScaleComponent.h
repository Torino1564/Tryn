#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ScaleComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(glm::vec3, scale, glm::vec3(1.0f, 1.0f, 1.0f));
			ZT_DEFINE_COMPONENT_VAR(float, color, 0.05f);
		);
	};
}