#pragma once
#include "Archetype_def.h"
#include <Core/src/ecs/EcsClass.h>

namespace tryn::ecs
{
	template <typename C>
	std::span<C> Archetype::GetComponentData()
	{
		const auto id = ZT_TYPE_UUID(C);
		for (auto [index, id_] : std::ranges::views::enumerate(components))
		{
			if (id_ == id)
			{
				return std::span<C>(
					reinterpret_cast<C*>(arrays[index].data()), upperLimit);
			}
		}
		return {};
	}

	template <typename... Cs>
	Archetype Archetype::Make(const ArchetypeManager& manager, const uint16_t uuid)
	{
		Archetype archetype(manager, uuid);
		archetype.AppendComponents<Cs...>();
		archetype.Resize(100);
		return archetype;
	}

	template <typename C>
	void Archetype::AppendComponents()
	{
		components.push_back(ZT_TYPE_UUID(C));
		arrays.push_back(componentManager.Wrapper<C>().MakeArray());
	}

	template <typename First, typename Second, typename... Rest>
	void Archetype::AppendComponents()
	{
		AppendComponents<First>();
		AppendComponents<Second, Rest...>();
	}


	template <typename... Cs>
	const Archetype& ArchetypeManager::AddArchetype()
	{
		return AddArchetype({ ZT_TYPE_UUID(Cs)... });
	}

	template <typename... Cs>
	Archetype* ArchetypeManager::GetArchetype() const
	{
		std::array<utl::UUID_t, sizeof...(Cs)> componentIDs = { ZT_TYPE_UUID(Cs)... };
		return GetArchetype(componentIDs);
	}

	template <typename ... Cs>
	std::span<std::tuple<std::span<Cs>...>> ArchetypeManager::GetComponentGroups()
	{
		std::vector<std::tuple<std::span<Cs>...>> result;

		const std::span<ArchetypeID> query = QueryArchetype<Cs...>();

		for (const auto archetypeID : query)
		{
			auto& archetype = archetypeBuffer[archetypeID];
			result.emplace_back(std::tuple<Cs...>{
				archetype.GetComponentData<Cs>()...,
			});
		}

		return result;
	}

	template <typename... Cs>
	std::span<ArchetypeID> ArchetypeManager::QueryArchetype() const
	{
		return QueryArchetype({ ZT_TYPE_UUID(Cs)... });
	}


}