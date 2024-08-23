#include "TrynPCH.h"
#include "PositionComponent.h"

namespace tryn::ecs::cmp
{
	const unsigned int PositionComponent::index = ComponentManager::RegisterComponent<PositionComponent>();

	const std::vector<utl::CTM::ElementData>& PositionComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
