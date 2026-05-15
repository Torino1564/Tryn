#include "InstancedModelParentComponent.h"
#include <Core/src/gfx/Model/InstancedModel.h>

namespace tryn::ecs
{
	InstancedModelParentComponent::InstancedModelParentComponent() = default;
	InstancedModelParentComponent::~InstancedModelParentComponent() = default;

	InstancedModelParentComponent::InstancedModelParentComponent(InstancedModelParentComponent&& rhs) noexcept = default;
	InstancedModelParentComponent& InstancedModelParentComponent::operator=(InstancedModelParentComponent&&) noexcept = default;
}
