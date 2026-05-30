#pragma once
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace tryn::gfx::ani
{
	struct VectorKey
	{
		VectorKey(double time, glm::vec3 pValue) : time(time), value(pValue) {}
		double time;
		glm::vec3 value;
	};

	struct QuatKey
	{
		QuatKey(double time, glm::quat pValue) : time(time), value(pValue) {}
		double time;
		glm::quat value;
	};
}