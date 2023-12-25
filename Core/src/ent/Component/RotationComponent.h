#pragma once
#include <Core/src/ent/Component/ComponentManager.h>

namespace tryn::ent::cmp
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