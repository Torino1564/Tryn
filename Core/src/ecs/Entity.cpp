#include "Entity.h"
#include <format>
#include <Core/src/gfx/ImguiManager.h>
#include <Core/src/ecs/cmp/Components.h>
#include <ranges>

namespace tryn::ecs
{
	Entity::Entity(std::string name)
		:
		name(std::move(name))
	{

	}
	Entity::~Entity()
	{
		pArchetype->Free(UUID);
	}
	std::span<int> Entity::GetComponents()
	{
		return std::span<int>(pArchetype->components.begin(), pArchetype->components.size());
	}
	void Entity::Instanciate(std::span<Entity> destination)
	{
		for (auto [instanceNum, ent] : std::ranges::views::enumerate(destination))
		{
			ent.name = name + "_" + std::to_string(instanceNum);
			ent.pArchetype = pArchetype;
			ent.UUID = pArchetype->ResolveEntityUUID();
		}
	}

		template <typename MapElement, ValidComponent C>
		struct PrintImGuiMemberVariable
		{
			void operator()(EntityID entityUUID)
			{
				auto pArchetype = ECS::Get().archetypeManager.GetArchetype(entityUUID.archetype);
				auto data = pArchetype->GetComponentData<C>();

				using VarNameFunc_t = typename MapElement::VarName_t;
				using TypeNameFunc_t = typename MapElement::TypeName_t;
				using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;

				VarNameFunc_t varNameFunc;
				TypeNameFunc_t typeNameFunc;
				ByteOffsetFunc_t byteOffsetFunc;

				if constexpr (std::is_same_v<typename MapElement::Type, float>)
				{
					auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityUUID.ID - 1]) + byteOffsetFunc());

					ImGui::DragFloat(std::format("{}: {}",typeNameFunc(), varNameFunc()).c_str(), pData);
				}
				else
				{
					ImGui::Text(std::format("{}: {}",typeNameFunc(), varNameFunc()).c_str());
				}
			}
		};

	void Entity::SpawnControlWindow()
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