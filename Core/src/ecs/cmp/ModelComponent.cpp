#include "TrynPCH.h"
#include "ModelComponent.h"
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ecs
{
	const unsigned int ModelComponent::index = ComponentManager::RegisterComponent<ModelComponent>();

	ModelComponent::SubresourceData::~SubresourceData() = default;

	ModelComponent::SubresourceData::SubresourceData() = default;

	const std::vector<utl::CTM::ElementData>& ModelComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
