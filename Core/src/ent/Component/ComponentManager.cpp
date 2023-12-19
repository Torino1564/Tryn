#include "ComponentManager.h"
#include <Core/src/ent/Entity.h>

namespace tryn::ent
{
	void tryn::ent::ComponentManager::ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex)
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
		if (nextFree == booker.npos)
		{
			Grow();
			nextFree = booker.find_next(bookerPointer);
		}
		booker.flip(nextFree);
		bookerPointer = nextFree;

		return {bookerPointer, UUID};
	}
	void Archetype::Free(EntityID entityID)
	{
		booker[entityID.ID].flip();
		bookerPointer = entityID.ID - 1;
	}
}

