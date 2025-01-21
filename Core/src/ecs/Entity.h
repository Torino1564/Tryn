#pragma once
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/Archetype.h>
#include <array>
#include "EntityID.h"
#include <Core/src/ser/StreamIO.h>

#include "SerializeLambda.h"
#include "Core/src/gfx/CoreGraphics.h"

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::ecs
{
	template <typename T>
	concept HasGfxPointer = requires (T t) {
		std::same_as<decltype(t.pGfx), gfx::IGraphics*>;
	};

	class Entity
	{
	public:
		Entity() = delete;

		virtual ~Entity();

		template <typename... Cs>
		static Entity CreateNew(ECS& ecs, std::string name = "?");
		std::span<utl::UUID_t> GetComponents();

		template <typename C>
		C& GetComponent();

		void Instanciate(std::span<Entity> destination);

		void SpawnControlWindow();

	protected:
		Entity(std::string name = "?");

		template <typename... Cs>
		void AddComponent(ECS& ecs);
		void AddComponent(ECS& ecs, std::span<utl::UUID_t> componentList);


		std::string name;
		EntityID UUID = {};
		Archetype* pArchetype = nullptr;
	};

	template<typename ...Cs>
	inline Entity Entity::CreateNew(ECS& ecs, std::string name)
	{
		Entity ent(std::move(name));
		ent.pArchetype = &ecs.GetArchetypeManager().GetArchetype<Cs...>();
		ent.UUID = ent.pArchetype->ResolveEntityUUID();
		return ent;
	}

	template<typename C>
	inline C& Entity::GetComponent()
	{
		auto data = pArchetype->GetComponentData<C>();
		return data[UUID.ID - 1];
	}

	template<typename ...Cs>
	inline void Entity::AddComponent(ECS& ecs)
	{
		std::array<utl::UUID_t, sizeof...(Cs)> newComponentIDs = { ZT_TYPE_UUID(Cs)... };
		AddComponent(ecs, newComponentIDs);
	}
}