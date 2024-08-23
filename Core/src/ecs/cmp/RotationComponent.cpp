#include "TrynPCH.h"
#include "RotationComponent.h"

namespace tryn::ecs::cmp
{
	const unsigned int RotationComponent::index = ComponentManager::RegisterComponent<RotationComponent>();

	const std::vector<utl::CTM::ElementData>& RotationComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
