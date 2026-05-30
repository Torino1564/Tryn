#pragma once
#include <glm/mat4x4.hpp>

namespace tryn::ecs
{
	struct TransformComponent
	{
		glm::mat4x4 transform;
	};
}