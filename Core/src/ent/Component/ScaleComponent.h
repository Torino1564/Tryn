#pragma once
#include <Core/src/ent/Component/ComponentManager.h>
#include <Core/third/glm/vec3.hpp>
namespace tryn::ent::cmp
{
	ZT_DEFINE_COMPONENT(ScaleComponent)
	{
		ZT_COMPONENT_FIELDS(
			glm::vec3 scale = glm::vec3( 1.0f,1.0f,1.0f );
		);
	};
}