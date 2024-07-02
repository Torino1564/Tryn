#pragma once
#include "ComponentManager.h"
#include <string>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ModelComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(std::unique_ptr<class gfx::Model>, pModel);
		);
	};
}