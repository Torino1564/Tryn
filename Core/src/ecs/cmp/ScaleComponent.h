#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ScaleComponent)
	{
		ZT_COMPONENT_FIELDS(
			glm::vec3 scale = glm::vec3( 1.0f,1.0f,1.0f );
		);
	};
}