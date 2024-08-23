#include "TrynPCH.h"
#include "PointLightComponent.h"

namespace tryn::ecs::cmp
{
	const unsigned int PointLightComponent::index = ComponentManager::RegisterComponent<PointLightComponent>();
	const std::vector<utl::CTM::ElementData>& PointLightComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
	const std::vector<utl::CTM::ElementData>& PointLightComponent::vec = GetReflectData_();
}

