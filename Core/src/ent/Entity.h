#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/ent/Component/ComponentManager.h>
#include <bitset>
#include <array>

namespace tryn::ent
{
	struct EntityID
	{
		std::uint32_t ID = 0;
		std::uint16_t archetype = 0;
	};

	class Entity
	{
	public:
		virtual ~Entity();
		Entity(std::optional<std::string_view> newName = std::nullopt);

		template <ValidComponent... Cs>
		static Entity CreateNew(std::optional<std::string_view> newName = std::nullopt)
		{
			Entity ent(newName);
			ent.pArchetype = ArchetypeManager::Get().GetArchetype<Cs...>();
			ent.UUID = ent.pArchetype->ResolveEntityUUID();
			return ent;
		}
		std::span<int> GetComponents()
		{
			return std::span<int>(pArchetype->components.begin(), pArchetype->components.size());
		}

		template <ValidComponent C>
		C::SubresourceData& GetComponent()
		{
			auto data = pArchetype->GetComponentData<C>();
			return data[UUID.ID - 1];
		}

		void Instanciate(std::span<Entity> destination)
		{
			for (auto [instanceNum, ent] : std::ranges::views::enumerate(destination))
			{
				ent.name = name + "_" + std::to_string(instanceNum);
				ent.pArchetype = pArchetype;
				ent.UUID = pArchetype->ResolveEntityUUID();
			}
		}
		void SpawnControlWindow();

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
		EntityID UUID = {};
		Archetype* pArchetype = nullptr;
	};
}