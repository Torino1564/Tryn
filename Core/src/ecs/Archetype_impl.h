#pragma once
#include "Archetype_def.h"
#include <Core/src/ecs/EcsClass.h>
#include <ranges>
#include "cmp/ComponentManager.h"

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
	Archetype Archetype::Make(ArchetypeManager& manager, const uint16_t uuid)
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
		const auto& wrapper = componentManager.PWrapper<C>();
		arrays.push_back(std::move(wrapper.MakeArray()));
	}

	template <typename First, typename Second, typename... Rest>
	void Archetype::AppendComponents()
	{
		AppendComponents<First>();
		AppendComponents<Second, Rest...>();
	}


	template <typename... Cs>
	Archetype& ArchetypeManager::AddArchetype()
	{
		return AddArchetype({ ZT_TYPE_UUID(Cs)... });
	}

	template <typename... Cs>
	Archetype& ArchetypeManager::GetArchetype()
	{
		std::array<utl::UUID_t, sizeof...(Cs)> componentIDs = { ZT_TYPE_UUID(Cs)... };
		return GetArchetype(componentIDs);
	}

	template <typename ... ACs>
	inline std::span<std::tuple<std::span<typename ACs::Component>...>> ArchetypeManager::GetComponentGroups()
	{
		const std::span<ArchetypeID> query = QueryArchetype<typename ACs::Component...>();

		auto result = pEcs->GetAllocator().MakeNewArray<std::tuple<std::span<typename ACs::Component>...>>(query.size());
		auto it = result.begin();
		for (const auto archetypeID : query)
		{
			auto& archetype = archetypeBuffer[archetypeID];
			*it++ = std::move(std::tuple<std::span<typename ACs::Component>...>{
				archetype.GetComponentData<typename ACs::Component>()...,
			});
		}

		return result;
	}

	template <typename... Cs>
	std::span<ArchetypeID> ArchetypeManager::QueryArchetype()
	{
		std::array<utl::UUID_t, sizeof...(Cs)> view = { ZT_TYPE_UUID(Cs)... };
		return QueryArchetype(std::span<utl::UUID_t>(view));
	}
}
