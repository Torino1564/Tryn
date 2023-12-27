#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(RotationComponent)
	{
		ZT_COMPONENT_FIELDS(
			float yaw = 0;
			float pitch = 0;
			float roll = 0;
		);
	};
}