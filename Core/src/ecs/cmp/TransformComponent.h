#pragma once
#include <Core/third/glm/mat4x4.hpp>

namespace tryn::ecs
{
	struct TransformComponent
	{
		glm::mat4x4 transform;
	};
}