#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include "Components/ComponentManager.h"
#include <Core/third/dynamic_bitset.hpp>
#include <bitset>
#include <array>

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity();
		IEntity();
		void SpawnControlWindow();
		void Mark();
		int GenerateID();
		template <Component ComponentType>
		void AddComponent(ComponentType component)
		{
			ComponentManager::Get().AddComponent<ComponentType>(UID, component);
			const auto type = ReverseComponentMap<ComponentType>::type;
			components[static_cast<int>(type)].flip();
		}
	protected:
		std::string name;
		int UID = -1;
		// Components
		std::bitset<static_cast<std::size_t>(ComponentType::Count)> components;
		// Entity ID
		static sul::dynamic_bitset<> IDbooker;
	};

	class BasicEntity : public IEntity
	{
	public:

		BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		BasicEntity(const BasicEntity&) = delete;
		BasicEntity& operator=(const BasicEntity&) = delete;

		BasicEntity(BasicEntity&&) = default;
		BasicEntity& operator=(BasicEntity&&) = default;
	};
}