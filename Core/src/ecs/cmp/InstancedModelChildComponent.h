#pragma once
#include <Core/src/gfx/Model/InstancedModel.h>

namespace tryn::ecs
{
	struct InstancedModelChildComponent
	{
		gfx::InstancedModelChild childModel = {};
	};
}