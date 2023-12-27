#include "Entity.h"
#include <format>

namespace tryn::ecs
{
	Entity::~Entity()
	{
		pArchetype->Free(UUID);
	}
	Entity::Entity(std::optional<std::string_view> newName)
		:
		name(newName.value_or(""))
	{
		
	}
	void Entity::SpawnControlWindow()
	{

	}
	/*
	int IEntity::GenerateID()
	{
		auto firstFree = IEntity::IDbooker.find_first();
		if (firstFree == sul::dynamic_bitset<>::npos)
		{
			const auto oldSize = IEntity::IDbooker.size();
			IEntity::IDbooker.resize(static_cast<int>((float)((oldSize) + 1) * 1.3f),true);
			IEntity::IDbooker[oldSize].flip();
			return oldSize;
		}
		else
		{
			IEntity::IDbooker[firstFree].flip();
			return firstFree;
		}
	}*/

}