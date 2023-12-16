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
		C::SubresourceData* GetComponent()
		{
			if (pComponents.size() < C::UUID || pComponents[C::UUID] == nullptr)
			{
				return nullptr;
			}
			else
			{
				return reinterpret_cast<C::SubresourceData*>(pComponents[C::UUID]);
			}
		}

		template <ValidComponent C>
		C::SubresourceData& AddComponent()
		{
			const auto componentUUID = C::UUID;
			if (pComponents.size() <= componentUUID)
			{
				pComponents.resize(componentUUID + 1);
			}

			if (pComponents[componentUUID] != nullptr)
			{
				return *reinterpret_cast<typename C::SubresourceData*>(pComponents[0]);
			}

			pComponents[componentUUID] = reinterpret_cast<void*>(ComponentManager::Get().AddComponent<C>(UID));

			return *(reinterpret_cast<C::SubresourceData*>(pComponents[C::UUID]));
		}

	protected:
		std::string name;
		int UID = -1;
		// Components
		std::vector<void*> pComponents;
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