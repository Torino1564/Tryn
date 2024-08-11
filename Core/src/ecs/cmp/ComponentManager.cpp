#include "TrynPCH.h"
#include "ComponentManager_def.h"
#include <Core/src/ecs/Entity.h>
#include "ActiveComponent.h"
#include "Core/src/ecs/Archetype.h"

namespace tryn::ecs
{
	ECS::ECS()
		:
		componentManager(ComponentManager::Get()), archetypeManager(ArchetypeManager::Get())
	{}

	void Archetype::InitializeUUID()
	{
		UUID = ECS::Get().archetypeManager.ResolveUUID();
	}
}

