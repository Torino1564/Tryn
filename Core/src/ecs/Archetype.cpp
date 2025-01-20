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
			auto pData = arrays[index][entityID.ID - 1];
			componentManager.Wrapper(componentUUID).Delete(pData);
		}
	}

	EntityID Archetype::	ResolveEntityUUID()
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

	const Archetype& ArchetypeManager::AddArchetype(const std::span<utl::UUID_t> componentUUIDs)
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

	auto& ArchetypeManager::GetArenaAllocator() const
	{
		return pEcs->GetAllocator();
	}

	const Archetype& ArchetypeManager::GetArchetype(const int archetypeCounter) const
	{
		trynass(archetypeCounter <= this->archetypeCounter);
		return archetypeBuffer[archetypeCounter];
	}

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

		trynass_msg(index < components.size());

		return arrays[index];
	}

	Archetype::Archetype(const ArchetypeManager& manager, const uint16_t uuid)
		:
	UUID(uuid),
	componentManager(manager.pEcs->GetComponentManager()),
	archetypeManager(manager)
	{
	}

	Archetype Archetype::Make(const ArchetypeManager& manager, const uint16_t uuid_, const std::span<utl::UUID_t> componentUUIDs)
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
		const auto query = QueryArchetype(componentUUIDs);
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
		}
	}

	std::span<ArchetypeID> ArchetypeManager::QueryArchetype(const std::span<utl::UUID_t> componentIDs) const
	{
		trynass(componentIDs.size() != 0);
		std::vector<const std::vector<ArchetypeID>&> vectors;
		
		vectors.reserve(componentIDs.size());

		auto smallest = &vectors.back();
		auto currentSize = std::numeric_limits<std::size_t>::infinity();
		for (const auto uuid : componentIDs)
		{
			vectors.emplace_back(archetypeTable.at(uuid));
			if (vectors.back().size() < currentSize)
			{
				smallest = &vectors.back();
				currentSize = smallest->size();
			}
		}

		std::vector<ArchetypeID> result;

		for (const auto archetypeID : *smallest)
		{
			auto i = 0;
			bool common = true;
			for (const auto uuid : componentIDs)
			{
				if (auto it = std::ranges::find(vectors[i], uuid); it == vectors[i].end())
				{
					common = false;
					break;
				}
				i++;
			}
			if (common)
				result.push_back(archetypeID);
		}

		return { result.begin(), result.size() };
	}

	const Archetype& ArchetypeManager::GetArchetype(const std::span<utl::UUID_t> components)
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
	void TypeSerializer<ecs::Archetype*>::Write(const StreamWriter& streamWriter, ecs::Archetype* const& data,
	const bool binary, const std::string& name)
	{
		std::vector<utl::UUID_t> sortedComponents = data->components;
		std::ranges::sort(sortedComponents);
		streamWriter.Serialize(sortedComponents, binary, name);
	}

	ecs::Archetype* TypeSerializer<ecs::Archetype*>::Read(const tryn::ser::StreamReader& streamReader,
		const bool binary, const ExtraDataPack* pExtraData)
	{
		/*ecs::ECS* pEcs = nullptr;
		pExtraData->Get("pEcs")((const void**)pEcs);

		std::vector<utl::UUID_t> componentUUIDs;

		streamReader.ReadSerialized(componentUUIDs, binary, pExtraData);

		return pEcs->GetArchetypeManager().GetArchetype(componentUUIDs);*/
		return nullptr;
	}

	void TypeSerializer<ecs::Archetype*>::Read(ecs::Archetype*& data, const tryn::ser::StreamReader& streamReader,
		const bool binary, const ExtraDataPack* pExtraData)
	{
		/*ecs::ECS* pEcs = nullptr;
		pExtraData->Get("pEcs")((const void**)pEcs);

		std::vector<utl::UUID_t> componentUUIDs;
		streamReader.ReadSerialized(componentUUIDs, binary, pExtraData);
		data = pEcs->GetArchetypeManager().GetArchetype(componentUUIDs);*/
	}
}
