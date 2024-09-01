#include "TrynPCH.h"
#include "BoneTransformsComponent.h"

namespace tryn::ecs
{
	const unsigned int BoneTransformsComponent::index = ComponentManager::RegisterComponent<BoneTransformsComponent>();

	const std::vector<utl::CTM::ElementData>& BoneTransformsComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
