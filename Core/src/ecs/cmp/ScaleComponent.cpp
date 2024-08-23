#include "TrynPCH.h"
#include "ScaleComponent.h"

namespace tryn::ecs::cmp
{
	const unsigned int ScaleComponent::index = ComponentManager::RegisterComponent<ScaleComponent>();

	const std::vector<utl::CTM::ElementData>& ScaleComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
