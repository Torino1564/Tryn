#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/gfx/Model/InstancedModel.h>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(InstancedModelParentComponent)
	{
		ZT_COMPONENT_FIELDS(
			gfx::InstancedModelParent parentModel;
		);
	};
}