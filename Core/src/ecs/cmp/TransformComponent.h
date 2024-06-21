#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/mat4x4.hpp>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(TransformComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(glm::mat4x4, transform);
		);
	};
}