#include "TrynPCH.h"
#include "Archetype.h"

#include "cmp/ComponentManager.h"

namespace tryn::ecs
{
	void Archetype::Free(const EntityID entityID)
	{
		booker[entityID.ID].flip();
		bookerPointer = entityID.ID - 1;

		for (auto [index, componentUUID] : std::views::enumerate(components))
		{
			auto pData = arrays[index][entityID.ID];
			componentManager.Wrapper(componentUUID).Delete(pData);
		}
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
		for (auto [index, componentUUID] : std::ranges::views::enumerate(components) )
		{
			auto pData = arrays[index][nextFree];
			componentManager.Wrapper(componentUUID).New(pData);
		}

		bookerPointer = (uint32_t)nextFree;
		if (bookerPointer > upperLimit)
		{
			upperLimit = (uint32_t)nextFree;
		}

		return {bookerPointer, UUID};
	}

	ArchetypeID ArchetypeManager::ResolveUUID()
	{
		return archetypeCounter++;
	}

	Archetype& ArchetypeManager::AddArchetype(const std::span<utl::UUID_t> componentUUIDs)
	{
		archetypeBuffer.emplace_back(Archetype::Make(*this, ResolveUUID(), componentUUIDs));

		auto& newlyAddedArchetype = archetypeBuffer.back();

		const auto archetypeID = newlyAddedArchetype.GetUUID();

		for (auto& componentUUID : newlyAddedArchetype.components)
		{
			archetypeTable[componentUUID].emplace_back(archetypeID);
		}

		return newlyAddedArchetype;
	}

	ArchetypeManager::ArchetypeManager(ECS* pEcs)
		: pEcs(pEcs)
	{
		archetypeBuffer.reserve(1000);
	}

	mem::ArenaAllocator<>& ArchetypeManager::GetArenaAllocator() const
	{
		return pEcs->GetAllocator();
	}

	Archetype& ArchetypeManager::GetArchetype(const int archetypeCounter)
	{
		trynass(archetypeCounter <= this->archetypeCounter);
		return archetypeBuffer[archetypeCounter];
	}

	//const Archetype& ArchetypeManager::GetArchetype(std::span<utl::UUID_t> componentUUIDs)
	//{
	//	trynass(componentUUIDs.size() != 0);
	//	std::vector<const std::vector<ArchetypeID>*> pVectors;
	//	for (const auto uuid : componentUUIDs)
	//	{
	//		auto it = archetypeTable.find(uuid);
	//		if (it == archetypeTable.end())
	//		{
	//			// This path is only reached if the component uuid doesnt have an archetype list
	//			// This means that the requested archetype does not exist and therefore has to be added

	//			return AddArchetype(componentUUIDs);
	//		}

	//		pVectors.push_back(&it->second);
	//	}
	//	std::vector<ArchetypeID> partialResult = *pVectors.front();
	//	for (const auto pVector : pVectors)
	//	{
	//		for (auto it = partialResult.begin(); it != partialResult.end(); )
	//		{
	//			if (std::ranges::find(*pVector, *it ) == pVector->end())
	//				it = partialResult.erase(it);
	//			else
	//				++it;
	//		}
	//		if (partialResult.empty())
	//			break;
	//	}

	//	// If the archetype doesnt exist, create a new one and return
	//	if (partialResult.empty())
	//		return AddArchetype(componentUUIDs);

	//	// Discard archetypes that have more than just the required components
	//	for (const auto archetypeID : partialResult)
	//	{
	//		if (auto& arch = archetypeBuffer[archetypeID]; arch.ComponentCount() == componentUUIDs.size())
	//			return arch;
	//	}

	//	// Create a new one if it doesnt exist
	//	return AddArchetype(componentUUIDs);
	//}

	ArchetypeID Archetype::GetUUID() const
	{
		return this->UUID;
	}

	size_t Archetype::ComponentCount() const
	{
		return components.size();
	}

	uint32_t Archetype::ComponentArraySize() const
	{
		return upperLimit;
	}

	void Archetype::Grow()
	{
		Resize((uint32_t)(booker.size() * 1.5f));
	}

	void Archetype::Resize(const std::uint32_t newSize)
	{
		booker.resize(newSize, true);
		for (auto [index, componentArray]: std::ranges::views::enumerate(arrays) )
		{
			componentArray.Resize(newSize);
		}
	}

	ComponentArray& Archetype::GetComponentArray(const utl::UUID_t uuid)
	{
		auto index = 0;
		for (auto componentUUID : components)
		{
			if (componentUUID == uuid)
				break;
			else
				index++;
		}

		trynass(index < components.size());

		return arrays[index];
	}

	void Archetype::EntityControlWindow(const EntityID id)
	{
		for (const auto uuid : components)
		{
			auto& wrapper = componentManager.Wrapper(uuid);
			auto& array = GetComponentArray(uuid);
			wrapper.ImGuiPrint(array[id.ID]);
		}
	}

	Archetype::Archetype(ArchetypeManager& manager, const uint16_t uuid)
		:
	UUID(uuid),
	componentManager(manager.pEcs->GetComponentManager()),
	archetypeManager(manager)
	{
	}

	Archetype Archetype::Make(ArchetypeManager& manager, const uint16_t uuid_, const std::span<utl::UUID_t> componentUUIDs)
	{
		Archetype archetype(manager,uuid_);
		archetype.components.reserve(componentUUIDs.size());
		for (const auto& uuid : componentUUIDs)
		{
			archetype.components.push_back(uuid);
			archetype.arrays.push_back(archetype.componentManager.Wrapper(uuid).MakeArray());
		}
		archetype.Resize(100);
		return archetype;
	}

	std::span<std::span<ComponentArray*>> ArchetypeManager::GetComponentGroups(
		const std::span<utl::UUID_t> componentUUIDs)
	{
		/*const auto query = QueryArchetype(componentUUIDs);
		std::vector<std::vector<ComponentArray*>> result;
		result.reserve(query.size());

		for (const auto archetypeID : query)
		{
			auto& archetype = archetypeBuffer[archetypeID];
			result.emplace_back();
			auto back = result.back();
			for (const auto& uuid : componentUUIDs)
			{
				back.emplace_back(&archetype.GetComponentArray(uuid));
			}
		}*/

		return {};
	}

	std::span<ArchetypeID> ArchetypeManager::QueryArchetype(const std::span<utl::UUID_t> componentIDs)
	{
		trynass(componentIDs.size() != 0);
		std::vector<ArchetypeID> const* pSmallest = nullptr;

		auto currentSize = std::numeric_limits<std::size_t>::infinity();
		for (const auto uuid : componentIDs)
		{
			if (const auto pVector = &archetypeTable[uuid]; pSmallest == nullptr || pVector->size() < currentSize)
			{
				pSmallest = pVector;
				currentSize = pSmallest->size();
			}
		}

		auto result = pEcs->GetAllocator().MakeNewArray<ArchetypeID>(currentSize);
		auto it = result.begin();
		for (const auto archetypeID : *pSmallest)
		{
			bool common = true;
			for (const auto uuid : componentIDs)
			{
				const auto componentList = archetypeBuffer[archetypeID].components;
				if (const auto& it = std::ranges::find(componentList, uuid); it == componentList.end())
				{
					common = false;
					break;
				}
			}
			if (common)
			{
				*it++ = archetypeID;
			}
		}

		return { result.begin(), it };
	}

	Archetype& ArchetypeManager::GetArchetype(const std::span<utl::UUID_t> components)
	{
		auto queriedArchetypes = QueryArchetype(components);

		for (const auto archetypeID : queriedArchetypes)
		{
			auto& archetype = archetypeBuffer[archetypeID];
			if (archetype.ComponentCount() == components.size())
			{
				return archetype;
			}
		}

		// No existing archetype was found, adding a new one
		return AddArchetype(components);
	}
}

namespace tryn::ser
{
	void Serialize(StreamIO& io, ecs::Archetype* data, const bool binary, const std::string& name)
	{
		std::ranges::sort(data->components);
		io.Field(&data->components, binary, name);
	}
}
