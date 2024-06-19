#include "Entity.h"
#include <format>
#include <Core/src/gfx/ImguiManager.h>

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
	void Entity::SpawnControlWindow()
	{
		if (ImGui::Begin(std::format("[{}] - Entity properties", name).c_str()))
		{
			ImGui::Text(std::format("Entity UUID: {}:{}", UUID.archetype, UUID.ID).c_str());
			ImGui::Text("Components:");
			if (ImGui::BeginCombo("Components", "Select a component"))
			{
				auto& componentSpan = pArchetype->componentSMPID;
				for (auto componentUUID : componentSpan)
				{
					
				}
				ImGui::EndCombo();
			}

			ImGui::End();
		}
	}

	template <unsigned N = 0>
	void PrintComponentData(const std::uint16_t componentSMPID)
	{
		if constexpr (N >= ComponentManager::GetComponentCount())
		{
			throw ComponentSMPException(L"Invalid componentSMPID. The requested component does not exist.");
		}
		if (N != componentSMPID)
		{
			PrintComponentData<N + 1>(componentSMPID);
		}
		else
		{

		}
	}
}