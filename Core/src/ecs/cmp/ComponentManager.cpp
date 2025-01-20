#include "TrynPCH.h"
#include "ComponentManager.h"

namespace tryn::ecs
{
	uint16_t ComponentManager::ComponentCount() const
	{
		return componentWrappers.size();
	}

	uint16_t ComponentManager::NextFreeAndIncrement()
	{
		return componentCount++;
	}

	ComponentManager::ComponentManager(const ECS* pEcs)
		: pEcs(pEcs)
	{
	}
}

