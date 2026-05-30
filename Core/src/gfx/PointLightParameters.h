#pragma once
#include "glm/vec3.hpp"

namespace tryn::gfx
{
	struct PointLightParameters
		{
			glm::vec3 viewLightPos;
			glm::vec3 diffuseColor;
			float diffuseIntensity;
			float constantAtt;
			float linearAtt;
			float quadraticAtt;
		};
}
