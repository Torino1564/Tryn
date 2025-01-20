#pragma once
#include <Core/third/glm/vec3.hpp>

namespace tryn::ecs
{
	struct ScaleComponent
	{
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	};
}