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
}