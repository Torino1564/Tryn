#pragma once
#include "ComponentManager.h"

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(ActiveComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(bool, active, true);
		);
	};
}