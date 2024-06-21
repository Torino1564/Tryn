#pragma once
#include "ComponentManager.h"
#include <Core/src/gfx/Model/InstancedModel.h>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(InstancedModelChildComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(gfx::InstancedModelChild, childModel);
		);
	};
}