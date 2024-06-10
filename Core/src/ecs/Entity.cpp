#include "Entity.h"
#include <format>

namespace tryn::ecs
{
	Entity::~Entity()
	{
		pArchetype->Free(UUID);
	}
	void Entity::SpawnControlWindow()
	{

	}
}