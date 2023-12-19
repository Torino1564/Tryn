#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/ent/Component/ComponentManager.h>
#include <bitset>
#include <array>

namespace tryn::ent
{
	class Entity
	{
	public:
		virtual ~Entity();
		Entity(std::string_view name);

		template <ValidComponent... Cs>
		static Entity CreateNew(std::string_view newName)
		{
			Entity ent(newName);
			ent.pArchetype = ArchetypeManager::Get().GetArchetype<Cs...>();
			return ent;
		}
		std::span<int> GetComponents()
		{
			return std::span<int>(pArchetype->components.begin(), pArchetype->components.size());
		}
		void SpawnControlWindow();
		void MarkForUpdate();

	protected:

		template <ValidComponent... Cs>
		void AddComponent()
		{
			auto& newComponentIDs = *ECS::Get().allocator.MakeNew<std::array<ComponentIndex,100>>();
			int index = 0;
			AddComponent<Cs...>(newComponentIDs, index);

			for (auto componentID : pArchetype->components)
			{
				newComponentIDs[++index] = componentID;
			}

			// Request the new Archetype
			auto newArchetype = ArchetypeManager::Get().GetArchetype(std::span<int>(newComponentIDs.begin(), newComponentIDs.size()));
		}

		template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void AddComponent_(std::array<ComponentIndex, 100>& newComponentIDs, int& index = 0)
		{
			AddComponent_<First>(newComponentIDs, index++);
			AddComponent_<Second, Rest...>(newComponentIDs, index);
		}

		template <ValidComponent C>
		void AddComponent_(std::array<ComponentIndex, 100>& newComponentIDs, int index = 0)
		{
			newComponentIDs[index] = C::UUID;
		}

		std::string name;
		int UID = -1;
		Archetype* pArchetype = nullptr;
	};
}