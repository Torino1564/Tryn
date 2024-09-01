#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tryn::gfx
{
	class InstancedModelParent;
}

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(InstancedModelParentComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(std::unique_ptr<gfx::InstancedModelParent>, pParentModel, nullptr);
		);
	};
}