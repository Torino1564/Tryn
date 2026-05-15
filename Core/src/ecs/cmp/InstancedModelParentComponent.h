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
		InstancedModelParentComponent();
		~InstancedModelParentComponent();

		InstancedModelParentComponent(const InstancedModelParentComponent&) = delete;
		InstancedModelParentComponent& operator=(const InstancedModelParentComponent&) = delete;

		InstancedModelParentComponent(InstancedModelParentComponent&&) noexcept;
		InstancedModelParentComponent& operator=(InstancedModelParentComponent&&) noexcept;

		std::unique_ptr<gfx::InstancedModelParent> pParentModel;
	};
}