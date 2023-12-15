#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/vec3.hpp>

namespace tryn::ent::cmp
{
	ZT_DEFINE_COMPONENT(PositionComponent)
	{
		ZT_COMPONENT_FIELDS(
			glm::vec3 position;
		);
	};
}