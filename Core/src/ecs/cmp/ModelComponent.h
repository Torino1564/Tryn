#pragma once
#include "ComponentManager.h"

namespace tryn::gfx
{
	class Model;
}

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ModelComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(std::unique_ptr<gfx::Model>, pModel);
		);
	};
}