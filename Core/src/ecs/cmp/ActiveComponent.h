#pragma once
#include "ComponentManager.h"

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ActiveComponent)
	{
		ZT_COMPONENT_FIELDS(
			bool active = true;
		);
	};
}