#include "TrynPCH.h"
#include "ComponentManager.h"

namespace tryn::ecs
{
	uint32_t ComponentManager::ComponentCount() const
	{
		return componentWrappers.size();
	}

	uint32_t ComponentManager::NextFreeAndIncrement()
	{
		return componentCount++;
	}

	ComponentManager::ComponentManager(const ECS* pEcs)
		: pEcs(pEcs)
	{
	}

	void ComponentManager::RegisterComponent(SoftComponentDescriptor&&)
	{
		// TODO: Implement Soft components
	}
}

