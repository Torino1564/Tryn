#pragma once
#include "Archetype_def.h"
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ecs/cmp/ComponentManager_def.h>

namespace tryn::ecs
{
	template <ValidComponent... Cs>
	void Archetype::FillComponentPointerTuple(std::tuple<std::span<typename Cs::SubresourceData>...>& container)
	{
		FillComponentPointerTupleImpl<0,Cs...>(container);
	}

	template <int N, ValidComponent... Cs>
	void Archetype::FillComponentPointerTupleImpl(std::tuple<std::span<typename Cs::SubresourceData>...>& container)
	{
		std::get<N>(container) = GetComponentData<NthTypeOf<N, Cs...>>();
		if constexpr (N < sizeof...(Cs) - 1)
		{
			FillComponentPointerTupleImpl<N + 1, Cs...>(container);
		}
	}

	template <ValidComponent C>
	std::span<typename C::SubresourceData> Archetype::GetComponentData()
	{
		for (auto [index, componentUUID] : std::ranges::views::enumerate(components))
		{
			if (componentUUID == C::UUID)
			{
				return std::span<typename C::SubresourceData>(
					reinterpret_cast<typename C::SubresourceData*>(bufferPtrs[index]->data()), upperLimit);
			}
		}
		return {};
	}

	template <ValidComponent... Cs>
	Archetype Archetype::Make()
	{
		Archetype archetype;
		archetype.InitializeUUID();
		archetype.AppendComponents<Cs...>();
		archetype.Resize(100);
		return archetype;
	}

	inline Archetype Archetype::Make(std::span<int> componentIDs)
	{
		Archetype archetype;
		archetype.InitializeUUID();
		archetype.components.reserve(componentIDs.size());
		for (auto i = 0 ; i < componentIDs.size() ; i++)
		{
			archetype.components.push_back(componentIDs[i]);
			archetype.bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
		}
		archetype.Resize(100);
		return archetype;
	}
	template <ValidComponent C>
	void Archetype::AppendComponents()
	{
		// components.push_back(C::UUID);
		components.push_back(C::UUID);
		bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
	}

	inline int Archetype::GetUUID() const
	{
		return this->UUID;
	}

	inline auto Archetype::ComponentCount() const
	{
		return components.size();
	}

	inline auto Archetype::ComponentArraySize() const
	{
		return upperLimit;
	}

	inline void Archetype::Grow()
	{
		Resize((uint32_t)(booker.size() * 1.5f));
	}

	inline void Archetype::Resize(std::uint32_t newSize)
	{
		booker.resize(newSize, true);
		for (auto [index, pBuffer]: std::ranges::views::enumerate(bufferPtrs) )
		{
			pBuffer->resize(newSize * ComponentManager::Get().GetComponentSize(components[index]));
		}
	}

	inline Archetype* ArchetypeManager::GetArchetype(const int archetypeCounter)
	{
		trynass(archetypeCounter <= this->archetypeCounter);
		return &archetypeBuffer[archetypeCounter];
	}

	inline Archetype* ArchetypeManager::AddArchetype(std::span<int> componentIDs)
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

	inline ArchetypeManager::ArchetypeManager()
	{
		archetypeBuffer.reserve(1000);
	}

	inline int ArchetypeManager::ResolveUUID()
	{
		return archetypeCounter++;
	}

	template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
	void Archetype::AppendComponents()
	{
		AppendComponents<First>();
		AppendComponents<Second, Rest...>();
	}

	template <ValidComponentWithAccessMode... Cs>
	std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> ArchetypeManager::GetComponentGroups()
	{
		auto archetypeQuery = QueryArchetype<typename Cs::ComponentType...>();
		mem::NativeArray<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> heterogeneusComponentSpanArray((uint32_t)archetypeQuery.size(), ECS::Get().allocator);

		for (auto i = 0 ; i < archetypeQuery.size() ; i++)
		{
			archetypeQuery[i]->FillComponentPointerTuple<typename Cs::ComponentType...>(heterogeneusComponentSpanArray[i]);
		}
			
		return std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>>(heterogeneusComponentSpanArray.begin(), heterogeneusComponentSpanArray.size());
	}

	inline EntityID Archetype::ResolveEntityUUID()
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
			ComponentData<Action::Fill>(&(*bufferPtrs[index])[nextFree], componentUUID);
		}

		bookerPointer = (uint32_t)nextFree;
		if (bookerPointer > upperLimit)
		{
			upperLimit = (uint32_t)nextFree;
		}

		return {bookerPointer, UUID};
	}

	inline void Archetype::Free(const EntityID entityID)
	{
		booker[entityID.ID].flip();
		bookerPointer = entityID.ID - 1;

		for (auto [index, componentUUID] : std::views::enumerate(components))
		{
			ComponentData<Action::Delete>(&(*bufferPtrs[index])[(entityID.ID - 1) * GetComponentInfo<ComponentInfo::Size>(componentUUID)], componentUUID);
		}
	}

	template <int arraySize, ValidComponent C>
	void ArchetypeManager::ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index)
	{
		componentIDs[index] = C::UUID;
	}

	template <int arraySize, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
	void ArchetypeManager::ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index)
	{
		ExtractComponentIDs<arraySize, First>(componentIDs, index++);
		ExtractComponentIDs<arraySize, Second, Rest...>(componentIDs, index);
	}

		template <ValidComponent... Cs>
	Archetype* ArchetypeManager::AddArchetype()
	{
		archetypeBuffer[archetypeCounter] = Archetype::Make<Cs...>();
		auto& newlyAddedArchetype = archetypeBuffer[archetypeCounter - 1];

		for (auto componentIndex : newlyAddedArchetype.components)
		{
			archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
		}
		return &newlyAddedArchetype;
	}

	template <ValidComponent... Cs>
	Archetype* ArchetypeManager::GetArchetype()
	{
		auto pComponentIDs = ECS::Get().allocator.MakeNew<std::array<int, sizeof...(Cs)>>();
		auto& componentIDs = *pComponentIDs;
		ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

		return GetArchetype(std::span<int>(componentIDs.begin(), componentIDs.size()));
	}

	template <ValidComponent... Cs>
	std::span<Archetype*> ArchetypeManager::QueryArchetype()
	{
		auto pComponentIDs = ECS::Get().allocator.MakeNew<std::array<int, sizeof...(Cs)>>();
		auto& componentIDs = *pComponentIDs;
		ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

		return QueryArchetype(std::span<int>(componentIDs.begin(), componentIDs.size()));
	}

	inline std::span<Archetype*> ArchetypeManager::QueryArchetype(std::span<ComponentIndex> componentIDs)
	{
		static std::vector<std::pair<Archetype*, int> > archetypeMap;
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			archetypeMap.resize(1000);
		}

		std::fill(archetypeMap.begin(), archetypeMap.end(), std::pair<Archetype*, int>{nullptr, 0});

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

	inline Archetype* ArchetypeManager::GetArchetype(std::span<ComponentIndex> components)
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
}