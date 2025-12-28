#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/Archetype.h>
#include <array>
#include "EntityID.h"
#include <Core/src/ser/StreamIO.h>

#include "Core/src/gfx/CoreGraphics.h"

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::ecs
{
	class Entity
	{
	public:
		Entity() = delete;
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		Entity(Entity&&) noexcept;
		Entity& operator=(Entity&&) noexcept;
		virtual ~Entity();

		template <typename... Cs>
		static Entity CreateNew(ECS& ecs, std::string name = "?");
		std::span<utl::UUID_t> GetComponents() const;

		template <typename C>
		C& GetComponent();

		template <typename C>
		const C& GetComponent() const;

		template <typename C>
		C& AddComponent(ECS& ecs);

		template <typename C>
		bool HasComponent() const;

		void Instanciate(std::span<Entity> destination) const;

		void SpawnControlWindow();

	protected:
		Entity(std::string name = "?");

		std::string name;
		EntityID UUID = {};
		Archetype* pArchetype = nullptr;
	};

	template<typename ...Cs>
	Entity Entity::CreateNew(ECS& ecs, std::string name)
	{
		Entity ent(std::move(name));
		ent.pArchetype = &ecs.GetArchetypeManager().GetArchetype< Cs...>();
		ent.UUID = ent.pArchetype->ResolveEntityUUID();
		return ent;
	}

	template<typename C>
	C& Entity::GetComponent()
	{
		auto data = pArchetype->GetComponentData<C>();
		return data[UUID.ID];
	}

	template <typename C>
	const C& Entity::GetComponent() const
	{
		auto data = pArchetype->GetComponentData<C>();
		return data[UUID.ID];
	}

	template <typename C>
	C& Entity::AddComponent(ECS& ecs)
	{
		auto& manager = ecs.GetArchetypeManager();
		auto currentComponents = pArchetype->components;
		currentComponents.emplace_back(ZT_TYPE_UUID(C));
		pArchetype = &manager.GetArchetype(currentComponents);
		UUID = manager.MoveEntity(*pArchetype, UUID);
		return GetComponent<C>();
	}

	template <typename C>
	bool Entity::HasComponent() const
	{
		return std::ranges::contains(pArchetype->components, ZT_TYPE_UUID(C));
	}
}