#include "Entity.h"
#include <format>

namespace tryn::ent
{
	IEntity::~IEntity()
	{
		IEntity::IDbooker[UID].flip();
	}
	IEntity::IEntity()
	{
		UID = GenerateID();
	}
	void IEntity::SpawnControlWindow()
	{

	}
	void IEntity::MarkForUpdate()
	{
		for (auto& pComponent : pComponents)
		{
			if (pComponent != nullptr)
			{
				auto activationProxy = reinterpret_cast<ActivationComponent::SubresourceData*>(pComponent);
				activationProxy->active = true;
			}
		}
	}
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
	}
	BasicEntity::BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale)
	{
		this->name = name;
	}
	BasicEntity::BasicEntity(std::string name)
	{
		this->name = name;
	}
}

sul::dynamic_bitset<> tryn::ent::IEntity::IDbooker;