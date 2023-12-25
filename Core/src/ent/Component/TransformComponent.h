#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/mat4x4.hpp>

namespace tryn::ent::cmp
{
	ZT_DEFINE_COMPONENT(TransformComponent)
	{
		ZT_COMPONENT_FIELDS(
			glm::mat4x4 transform;
		);
	};
}