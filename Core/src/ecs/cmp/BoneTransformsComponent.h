#pragma once
#include <span>
#include <glm/glm.hpp>

namespace tryn::ecs
{
	struct BoneTransformsComponent
	{
		std::span<glm::mat4> transforms = {};
	};
}