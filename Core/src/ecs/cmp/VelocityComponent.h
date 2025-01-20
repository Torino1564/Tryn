#pragma once
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs
{
	struct VelocityComponent
	{
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	};
}