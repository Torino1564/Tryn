#include "TrynPCH.h"
#include "InstancedModelChildComponent.h"

namespace tryn::ecs
{
	const unsigned int InstancedModelChildComponent::index = ComponentManager::RegisterComponent<InstancedModelChildComponent>();

	const std::vector<utl::CTM::ElementData>& InstancedModelChildComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
