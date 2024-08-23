#include "TrynPCH.h"
#include "AnimatedComponent.h"

namespace tryn::ecs::cmp
{
	const unsigned int AnimatedComponent::index = ComponentManager::RegisterComponent<AnimatedComponent>();

	const std::vector<utl::CTM::ElementData>& AnimatedComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
