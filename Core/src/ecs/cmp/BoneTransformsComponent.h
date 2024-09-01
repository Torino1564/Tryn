#pragma once
#include "ComponentManager.h"
#include <span>
#include <Core/third/glm/glm.hpp>
namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(BoneTransformsComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(std::span<glm::mat4>, transforms, {});
		);
	};
}