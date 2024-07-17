#include "ComponentManager_def.h"
#include <Core/src/ecs/Entity.h>
#include "ActiveComponent.h"
#include "Core/src/ecs/Archetype.h"

namespace tryn::ecs
{
	void tryn::ecs::ComponentManager::ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex)
	{

	}

	ComponentManager::ComponentManager()
	{
		RegisterComponents();
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

