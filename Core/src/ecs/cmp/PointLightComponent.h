#pragma once
#include "ComponentManager.h"
#include <Core/third/glm/vec3.hpp>
#include <Core/src/gfx/PointLight.h>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(PointLightComponent)
	{
		ZT_COMPONENT_FIELDS(
			gfx::PointLight::Parameters parameters;
		);
	};
}