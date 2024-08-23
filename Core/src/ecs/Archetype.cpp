#include "TrynPCH.h"
#include "Archetype.h"

namespace tryn::ecs
{
	void Archetype::Free(const EntityID entityID)
	{
		booker[entityID.ID].flip();
		bookerPointer = entityID.ID - 1;

		for (auto [index, componentUUID] : std::views::enumerate(componentUUIDs))
		{
			ComponentData(&(*bufferPtrs[index])[(entityID.ID - 1) * GetComponentInfo<ComponentInfo::Size>(componentUUID)], componentUUID, Action::Delete);
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
		for (auto [index, componentUUID] : std::ranges::views::enumerate(componentUUIDs) )
		{
			ComponentData(&(*bufferPtrs[index])[nextFree], componentUUID, Action::Fill);
		}

		bookerPointer = (uint32_t)nextFree;
		if (bookerPointer > upperLimit)
		{
			upperLimit = (uint32_t)nextFree;
		}

		return {bookerPointer, UUID};
	}

	ArchetypeManager::ArchetypeManager()
	{
		archetypeBuffer.reserve(1000);
	}

	int ArchetypeManager::ResolveUUID()
	{
		return archetypeCounter++;
	}

	Archetype* ArchetypeManager::AddArchetype(std::span<utl::UUID_t> componentIDs)
	{
		archetypeBuffer.emplace_back(Archetype::Make(componentIDs));
		auto& newlyAddedArchetype = archetypeBuffer.back();

		for (auto componentIndex : newlyAddedArchetype.components)
		{
			if (archetypeTable.size() < componentIndex + 1)
			{
				archetypeTable.resize(componentIndex + 1);
			}
			archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
		}
		return &newlyAddedArchetype;
	}

	Archetype* ArchetypeManager::GetArchetype(const int archetypeCounter)
	{
		trynass(archetypeCounter <= this->archetypeCounter);
		return &archetypeBuffer[archetypeCounter];
	}

	int Archetype::GetUUID() const
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

	void Archetype::Resize(std::uint32_t newSize)
	{
		booker.resize(newSize, true);
		for (auto [index, pBuffer]: std::ranges::views::enumerate(bufferPtrs) )
		{
			pBuffer->resize(newSize * GetComponentInfo<ComponentInfo::Size>(componentUUIDs[index]));
		}
	}

	Archetype Archetype::Make(std::span<utl::UUID_t> componentIDs)
	{
		Archetype archetype;
		archetype.InitializeUUID();
		archetype.components.reserve(componentIDs.size());
		for (auto i = 0 ; i < componentIDs.size() ; i++)
		{
			archetype.componentUUIDs.push_back(componentIDs[i]);
			archetype.components.push_back(GetComponentInfo<ComponentInfo::Index>(componentIDs[i]));
			archetype.bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
		}
		archetype.Resize(100);

		archetype.InitSortedComponentUUIDs();

		return archetype;
	}

	std::span<Archetype*> ArchetypeManager::QueryArchetype(const std::span<utl::UUID_t> componentIDs) const
	{
		static std::vector<std::pair<Archetype*, int> > archetypeMap;
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			archetypeMap.resize(1000);
		}

		std::ranges::fill(archetypeMap.begin(), archetypeMap.end(), std::pair<Archetype*, int>{nullptr, 0});

		auto pResult = ECS::Get().allocator.MakeNew<std::array<Archetype*, 100>>();
		auto& result = *pResult;

		for (int i = 0 ; i < componentIDs.size() ; i++)
		{
			if (componentIDs[i] < archetypeTable.size())
			{
				for (auto& archetype : archetypeTable[componentIDs[i]])
				{
					auto& ref = archetypeMap[archetype->GetUUID()];
					ref.first = archetype;
					ref.second++;
				}
			}
		}

		int resultCounter = 0;
		for (auto& [pType, counter] : archetypeMap)
		{
			if (counter == componentIDs.size())
			{
				result[resultCounter++] = pType;
			}
		}

		return std::span<Archetype*>(result.begin(), resultCounter);
	}

	Archetype* ArchetypeManager::GetArchetype(const std::span<utl::UUID_t> components)
	{
		auto queriedArchetypes = QueryArchetype(components);

		for (auto i = 0 ; i < queriedArchetypes.size() ; i++)
		{
			auto pArchetype = queriedArchetypes[i];
			if (pArchetype != nullptr && pArchetype->ComponentCount() == components.size())
			{
				return pArchetype;
			}
		}

		// No existing archetype was found, adding a new one
		return AddArchetype(components);
	}

	void Archetype::InitSortedComponentUUIDs()
	{
		sortedComponentUUIDs.resize(ComponentManager::GetComponentCount());
		for (auto [index, componentIndex] : std::ranges::views::enumerate(components))
		{
			sortedComponentUUIDs[componentIndex] = componentUUIDs[index];
		}
	}
}

namespace tryn::ser
{
	void TypeSerializer<ecs::Archetype*>::Write(const StreamWriter& streamWriter, ecs::Archetype* const& data,
	const bool binary, const std::string& name)
	{
		std::vector<utl::UUID_t> sortedComponents = data->componentUUIDs;
		std::ranges::sort(sortedComponents);
		streamWriter.Serialize(sortedComponents, binary, name);
	}

	ecs::Archetype* TypeSerializer<ecs::Archetype*>::Read(const tryn::ser::StreamReader& streamReader,
		const bool binary, const ExtraDataPack* pExtraData)
	{
		std::vector<utl::UUID_t> componentUUIDs;

		streamReader.ReadSerialized(componentUUIDs, binary, pExtraData);

		return ecs::ECS::Get().archetypeManager.GetArchetype(componentUUIDs);
	}

	void TypeSerializer<ecs::Archetype*>::Read(ecs::Archetype*& data, const tryn::ser::StreamReader& streamReader,
		const bool binary, const ExtraDataPack* pExtraData)
	{
		std::vector<utl::UUID_t> componentUUIDs;
		streamReader.ReadSerialized(componentUUIDs, binary, pExtraData);
		data = ecs::ECS::Get().archetypeManager.GetArchetype(componentUUIDs);
	}
}
