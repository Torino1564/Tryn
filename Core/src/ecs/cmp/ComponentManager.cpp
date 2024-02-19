#include "ComponentManager.h"
#include <Core/src/ecs/Entity.h>

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

	EntityID Archetype::ResolveEntityUUID()
	{
		auto nextFree = booker.find_next(bookerPointer);
		while (nextFree == booker.npos)
		{
			Grow();
			nextFree = booker.find_next(bookerPointer);
		}
		booker.flip(nextFree);
		bookerPointer = (uint32_t)nextFree;
		if (bookerPointer > upperLimit)
		{
			upperLimit = (uint32_t)nextFree;
		}

		return {bookerPointer, UUID};
	}
	void Archetype::Free(EntityID entityID)
	{
		booker[entityID.ID].flip();
		bookerPointer = entityID.ID - 1;
	}
}

