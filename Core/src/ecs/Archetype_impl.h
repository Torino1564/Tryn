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

	template <ValidComponent C, bool Index>
	std::span<typename C::SubresourceData> Archetype::GetComponentData()
	{
		for (auto [index, componentUUID] : std::ranges::views::enumerate(components))
		{
			if (componentUUID == (Index ? C::index : C::UUID))
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

	template <ValidComponent C>
	void Archetype::AppendComponents()
	{
		// components.push_back(C::UUID);
		components.push_back(C::UUID);
		bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
	}

	template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
	void Archetype::AppendComponents()
	{
		AppendComponents<First>();
		AppendComponents<Second, Rest...>();
	}

	template <ValidComponentWithAccessMode... Cs>
	std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> ArchetypeManager::GetComponentGroups() const
	{
		auto archetypeQuery = QueryArchetype<typename Cs::ComponentType...>();
		mem::NativeArray<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> heterogeneusComponentSpanArray((uint32_t)archetypeQuery.size(), pEcs->GetAllocator());

		for (auto i = 0 ; i < archetypeQuery.size() ; i++)
		{
			archetypeQuery[i]->FillComponentPointerTuple<typename Cs::ComponentType...>(heterogeneusComponentSpanArray[i]);
		}
			
		return std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>>(heterogeneusComponentSpanArray.begin(), heterogeneusComponentSpanArray.size());
	}

	template <int arraySize, ArchetypeManager::ComponentIdentifier Type, ValidComponent C>
	void ArchetypeManager::ExtractComponentIDs(std::array<utl::UUID_t, arraySize>& componentIDs, int index)
	{
		componentIDs[index] = (Type == Index ? C::index : C::UUID);
	}

	template <int arraySize, ArchetypeManager::ComponentIdentifier Type, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
	void ArchetypeManager::ExtractComponentIDs(std::array<utl::UUID_t, arraySize>& componentIDs, int index)
	{
		ExtractComponentIDs<arraySize, Type, First>(componentIDs, index++);
		ExtractComponentIDs<arraySize, Type, Second, Rest...>(componentIDs, index);
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
	Archetype* ArchetypeManager::GetArchetype() const
	{
		auto pComponentIDs = pEcs->GetAllocator().MakeNew<std::array<utl::UUID_t, sizeof...(Cs)>>();
		auto& componentIDs = *pComponentIDs;
		ExtractComponentIDs<sizeof...(Cs), UUID, Cs...>(componentIDs);

		return GetArchetype(std::span<utl::UUID_t>(componentIDs.begin(), componentIDs.size()));
	}

	template <ValidComponent... Cs>
	std::span<Archetype*> ArchetypeManager::QueryArchetype() const
	{
		auto pComponentIDs = pEcs->GetAllocator().MakeNew<std::array<utl::UUID_t, sizeof...(Cs)>>();
		auto& componentIDs = *pComponentIDs;
		ExtractComponentIDs<sizeof...(Cs), Index, Cs...>(componentIDs);

		return QueryArchetype(std::span<utl::UUID_t>(componentIDs.begin(), componentIDs.size()));
	}
}