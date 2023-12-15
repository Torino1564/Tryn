#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/ent/Component/ComponentManager.h>
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
		void MarkForUpdate();
		int GenerateID();

		template <ValidComponent C>
		C::SubresourceData& GetComponent()
		{
			for (auto& [componentUUID, componentIndex] : componentIds)
			{
				if (componentUUID == C::UUID)
				{
					return ComponentManager::Get().GetComponent<C>(componentIndex);
				}
			}
		}

		template <ValidComponent C>
		C::SubresourceData& AddComponent()
		{
			for (auto& [componentUUID, componentIndex] : componentIds)
			{
				if (componentUUID == C::UUID)
				{
					trylog.warn(L"Did not add component. Returning the existing component instead!");
					return ComponentManager::Get().GetComponent<C>(componentIndex);
				}
			}
			const auto index = ComponentManager::Get().AddComponent<C>();
			componentIds.push_back({C::UUID, index});

			return ComponentManager::Get().GetComponent<C>(index);
		}

	protected:
		std::string name;
		int UID = -1;
		// Components
		std::vector<std::pair<int, int>> componentIds;
		// Entity ID
		static sul::dynamic_bitset<> IDbooker;
	};

	class BasicEntity : public IEntity
	{
	public:
		template <ValidComponent ... Args>
		BasicEntity(std::string name)
		{
			this->name = name;
			AddMultipleComponents<Args...>();
		}

		BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		BasicEntity(std::string name);
		BasicEntity(const BasicEntity&) = delete;
		BasicEntity& operator=(const BasicEntity&) = delete;

		BasicEntity(BasicEntity&&) = default;
		BasicEntity& operator=(BasicEntity&&) = default;

	private:
		template <ValidComponent First, ValidComponent ... Args>
		void AddMultipleComponents()
		{
			AddComponent<First>();
			AddMultipleComponents<Args...>();
		}
		template <ValidComponent Last>
		void AddMultipleComponents()
		{
			return;
		}
	};
}