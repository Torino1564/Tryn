#include "TrynPCH.h"
#include "ActiveComponent.h"

namespace tryn::ecs
{
	const unsigned int ActiveComponent::index = ComponentManager::RegisterComponent<ActiveComponent>();

	const std::vector<utl::CTM::ElementData>& ActiveComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
