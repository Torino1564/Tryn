#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(RotationComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(float, yaw, 0);
			ZT_DEFINE_COMPONENT_VAR(float, pitch, 0);
			ZT_DEFINE_COMPONENT_VAR(float, roll, 0);
		);
	};
}