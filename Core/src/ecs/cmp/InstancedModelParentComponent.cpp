#include "TrynPCH.h"
#include "InstancedModelParentComponent.h"
#include <Core/src/gfx/Model/InstancedModel.h>

namespace tryn::ecs
{
	const unsigned int InstancedModelParentComponent::index = ComponentManager::RegisterComponent<InstancedModelParentComponent>();

	const std::vector<utl::CTM::ElementData>& InstancedModelParentComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
