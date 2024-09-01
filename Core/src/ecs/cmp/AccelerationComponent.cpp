#include "TrynPCH.h"
#include "AccelerationComponent.h"

namespace tryn::ecs
{
	const unsigned int AccelerationComponent::index = ComponentManager::RegisterComponent<AccelerationComponent>();

	const std::vector<utl::CTM::ElementData>& AccelerationComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
