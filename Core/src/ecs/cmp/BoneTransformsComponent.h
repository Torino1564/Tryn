#pragma once
#include "ComponentManager.h"
#include <span>
#include <Core/third/glm/glm.hpp>
namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(BoneTransformsComponent)
	{
		ZT_COMPONENT_FIELDS(
			std::span<glm::mat4> transforms;
		);
	};
}