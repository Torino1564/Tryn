#include "TrynPCH.h"
#include "Entity.h"
#include <Core/src/ecs/cmp/Components.h>
#include <ranges>

namespace tryn::ecs
{
	Entity::Entity(std::string name)
		:
		name(std::move(name))
	{

	}

	void Entity::AddComponent(ECS& ecs, std::span<utl::UUID_t> componentList)
	{

	}

	Entity::~Entity()
	{
		pArchetype->Free(UUID);
	}

	std::span<utl::UUID_t> Entity::GetComponents()
	{
		return std::span(pArchetype->components.begin(), pArchetype->components.size());
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
			
			pArchetype->EntityControlWindow(UUID);

			ImGui::End();
		}
	}
}
