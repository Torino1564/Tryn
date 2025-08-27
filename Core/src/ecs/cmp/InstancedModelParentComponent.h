#pragma once
#include <memory>

namespace tryn::gfx
{
	class InstancedModelParent;
}

namespace tryn::ecs
{
	struct InstancedModelParentComponent
	{
		std::unique_ptr<gfx::InstancedModelParent> pParentModel = nullptr;
	};
}