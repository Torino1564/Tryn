#include "Entity.h"
#include <Core/src/ent/Components/Component.h>
#include <format>

namespace tryn::ent
{
	IEntity::~IEntity()
	{
		for (int i = 0 ; i < components.size() ; i++)
		{
			if (components[i] == true)
			{
				
			}
		}
	}
	void IEntity::SpawnControlWindow()
	{

	}
	void IEntity::Update(double dt)
	{

	}
	int IEntity::GenerateID()
	{
		auto firstFree = IEntity::IDbooker.find_first();
		if (firstFree == sul::dynamic_bitset<>::npos)
		{
			const auto oldSize = IEntity::IDbooker.size();
			IEntity::IDbooker.resize(static_cast<int>((float)(oldSize) * 1.3f),true);
			IEntity::IDbooker[oldSize].flip();
			return oldSize;
		}
		else
		{
			IEntity::IDbooker[firstFree].flip();
			return firstFree;
		}
	}
	BasicEntity::BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale)
	{
		this->name = name;
		AddComponent(std::move(ModelComponent(gfx, path, scale)));
	}
}

sul::dynamic_bitset<> tryn::ent::IEntity::IDbooker;