#pragma once
#include <Core/src/gfx/PointLightParameters.h>

namespace tryn::ecs
{
	struct PointLightComponent
	{
		gfx::PointLightParameters parameters;
	};
}