#include "TrynPCH.h"
#include "VelocityComponent.h"

namespace tryn::ecs::cmp
{
	const unsigned int VelocityComponent::index = ComponentManager::RegisterComponent<VelocityComponent>();

	const std::vector<utl::CTM::ElementData>& VelocityComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<ComponentType>();
		return singleton;
	}
}