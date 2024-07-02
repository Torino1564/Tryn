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

	EntityID Archetype::ResolveEntityUUID()
	{
		auto nextFree = booker.find_next(bookerPointer);
		while (nextFree == booker.npos)
		{
			Grow();
			nextFree = booker.find_next(bookerPointer);
		}
		booker.flip(nextFree);

		// Default initialize the subresource data structure

		bookerPointer = (uint32_t)nextFree;
		if (bookerPointer > upperLimit)
		{
			upperLimit = (uint32_t)nextFree;
		}

		return {bookerPointer, UUID};
	}
	void Archetype::Free(const EntityID entityID)
	{
		booker[entityID.ID].flip();
		bookerPointer = entityID.ID - 1;
		std::byte* pData = nullptr;

		for (auto [index, componentUUID] : std::views::enumerate(components))
		{
			if ((uint16_t)componentUUID != cmp::ActiveComponent::UUID)
				continue;

			pData = bufferPtrs[index]->data();
			break;
		}
		// TODO: Finish this
	}
}

