#include "ComponentManager.h"
#include <Core/src/ecs/Entity.h>

#include "ActiveComponent.h"

namespace tryn::ecs
{
	void tryn::ecs::ComponentManager::ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex)
	{

	}

	ECS::ECS()
		:
		componentManager(ComponentManager::Get()), archetypeManager(ArchetypeManager::Get())
	{}

	void Archetype::InitializeUUID()
	{
		UUID = ECS::Get().archetypeManager.ResolveUUID();
	}
}

