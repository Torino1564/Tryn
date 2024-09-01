#include "TrynPCH.h"
#include "TransformComponent.h"

namespace tryn::ecs
{
	const unsigned int TransformComponent::index = ComponentManager::RegisterComponent<TransformComponent>();

	const std::vector<utl::CTM::ElementData>& TransformComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
