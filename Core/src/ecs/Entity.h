#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <array>

namespace tryn::ecs
{
	struct EntityID
	{
		std::uint32_t ID = 0;
		std::uint16_t archetype = 0;
	};

	class Entity
	{
	public:
		Entity(std::string name = "?");
		virtual ~Entity();

		template <ValidComponent... Cs>
		static Entity CreateNew(std::string name = "?");
		std::span<int> GetComponents();

		template <ValidComponent C>
		C::SubresourceData& GetComponent();

		void Instanciate(std::span<Entity> destination);

		void SpawnControlWindow();

	protected:
		template <ValidComponent... Cs>
		void AddComponent();

		template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void AddComponent_(std::array<ComponentIndex, 100>& newComponentIDs, int& index = 0);

		template <ValidComponent C>
		void AddComponent_(std::array<ComponentIndex, 100>& newComponentIDs, int index = 0);

		std::string name;
		EntityID UUID = {};
		Archetype* pArchetype = nullptr;

	// Component selection stuff
	private:
		std::vector<std::uint8_t> selectedComponents;
	};

	template <typename MapElement, ValidComponent C>
	struct PrintImGuiMemberVariable
	{
		void operator()(EntityID entityUUID)
		{
			auto pArchetype = ECS::Get().archetypeManager.GetArchetype(entityUUID.archetype);
			auto data = pArchetype->GetComponentData<C>();

			using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
			ByteOffsetFunc_t byteOffsetFunc;

			auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityUUID.ID - 1]) + byteOffsetFunc());

			cmp::ImGuiPrintElement<MapElement>::Print(pData);
		}
	};

	template<ValidComponent ...Cs>
	inline Entity Entity::CreateNew(std::string name)
	{
		Entity ent(std::move(name));
		ent.pArchetype = ArchetypeManager::Get().GetArchetype<Cs...>();
		ent.UUID = ent.pArchetype->ResolveEntityUUID();
		return ent;
	}

	template<ValidComponent C>
	inline C::SubresourceData& Entity::GetComponent()
	{
		auto data = pArchetype->GetComponentData<C>();
		return data[UUID.ID - 1];
	}

	template<ValidComponent ...Cs>
	inline void Entity::AddComponent()
	{
		auto& newComponentIDs = *ECS::Get().allocator.MakeNew<std::array<ComponentIndex, 100>>();
		int index = 0;
		AddComponent<Cs...>(newComponentIDs, index);

		for (auto componentID : pArchetype->components)
		{
			newComponentIDs[++index] = componentID;
		}

		// Request the new Archetype
		auto newArchetype = ArchetypeManager::Get().GetArchetype(std::span<int>(newComponentIDs.begin(), newComponentIDs.size()));
	}

	template<ValidComponent First, ValidComponent Second, ValidComponent ...Rest>
	inline void Entity::AddComponent_(std::array<ComponentIndex, 100>& newComponentIDs, int& index)
	{
		AddComponent_<First>(newComponentIDs, index++);
		AddComponent_<Second, Rest...>(newComponentIDs, index);
	}

	template<ValidComponent C>
	inline void Entity::AddComponent_(std::array<ComponentIndex, 100>& newComponentIDs, int index)
	{
		newComponentIDs[index] = C::UUID;
	}

	inline void Entity::SpawnControlWindow()
	{
		if (ImGui::Begin(std::format("[{}] - Entity properties", name).c_str()))
		{
			ImGui::Text(std::format("Entity UUID: {}:{}", UUID.archetype, UUID.ID).c_str());
			ImGui::Text("Components:");
			auto& componentSpan = pArchetype->components;
			if (ImGui::BeginCombo("Components", "Select a component"))
			{
				if (selectedComponents.size() != ComponentManager::GetComponentCount())
				{
					selectedComponents.resize(ComponentManager::GetComponentCount(), false);
				}
				for (auto componentUUID : componentSpan)
				{
					ImGui::Selectable(ComponentManager::GetComponentName(componentUUID), reinterpret_cast<bool*>(&selectedComponents[componentUUID]));
				}
				ImGui::EndCombo();
			}
			for (auto [UUID, selected] : std::ranges::enumerate_view(selectedComponents))
			{
				if (!selected)
				{
					continue;
				}
				ComponentManager::IterateComponentMembers<PrintImGuiMemberVariable>(UUID, this->UUID);
			}
			ImGui::End();
		}
	}

}