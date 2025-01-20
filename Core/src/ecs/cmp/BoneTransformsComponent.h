#pragma once
#include <span>
#include <Core/third/glm/glm.hpp>

namespace tryn::ecs
{
	struct BoneTransformsComponent
	{
		std::span<glm::mat4> transforms = {};
	};
}