#pragma once
#include <memory>
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/Archetype.h>
#include <array>
#include "EntityID.h"
#include <Core/src/ser/StreamIO.h>

#include "SerializeLambda.h"

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
		Entity(std::string name = "?");

		virtual ~Entity();

		template <ValidComponent... Cs>
		static Entity CreateNew(std::string name = "?");
		std::span<unsigned int> GetComponents();

		template <ValidComponent C>
		typename C::SubresourceData& GetComponent();

		void Instanciate(std::span<Entity> destination);

		void SpawnControlWindow();

	public:
		// Serializer 
		struct Serializer : public ser::Serializer<Entity>
		{
			static void Write(const ser::StreamWriter& streamWriter, const Entity& data, const bool binary = true, const std::string& name = "");

			static Entity Read(const ser::StreamReader& streamReader, const bool binary = true, const ser::ExtraDataPack* pExtraData = nullptr);
		};

	protected:
		template <auto Tag = []{}>
		void DeleteImpl_() const
		{
			pArchetype->Free(UUID);
		}
		template <ValidComponent... Cs>
		void AddComponent();

		template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void AddComponent_(std::array<utl::UUID_t, 100>& newComponentIDs, int& index = 0);

		template <ValidComponent C>
		void AddComponent_(std::array<utl::UUID_t, 100>& newComponentIDs, int index = 0);

		std::string name;
		EntityID UUID = {};
		Archetype* pArchetype = nullptr;

	// Component selection stuff
	private:
		std::vector<std::uint8_t> selectedComponents;
	};

	template<ValidComponent ...Cs>
	inline Entity Entity::CreateNew(std::string name)
	{
		Entity ent(std::move(name));
		ent.pArchetype = ArchetypeManager::Get().GetArchetype<Cs...>();
		ent.UUID = ent.pArchetype->ResolveEntityUUID();
		return ent;
	}

	template<ValidComponent C>
	inline C::SubresourceData& Entity::GetComponent()
	{
		auto data = pArchetype->GetComponentData<C>();
		return data[UUID.ID - 1];
	}

	template<ValidComponent ...Cs>
	inline void Entity::AddComponent()
	{
		auto& newComponentIDs = *ECS::Get().allocator.MakeNew<std::array<utl::UUID_t, 100>>();
		int index = 0;
		AddComponent<Cs...>(newComponentIDs, index);

		for (auto componentID : pArchetype->components)
		{
			newComponentIDs[++index] = componentID;
		}

		// Request the new Archetype
		auto newArchetype = ArchetypeManager::Get().GetArchetype(std::span<utl::UUID_t>(newComponentIDs.begin(), newComponentIDs.size()));
	}

	template<ValidComponent First, ValidComponent Second, ValidComponent ...Rest>
	inline void Entity::AddComponent_(std::array<utl::UUID_t, 100>& newComponentIDs, int& index)
	{
		AddComponent_<First>(newComponentIDs, index++);
		AddComponent_<Second, Rest...>(newComponentIDs, index);
	}

	template<ValidComponent C>
	inline void Entity::AddComponent_(std::array<utl::UUID_t, 100>& newComponentIDs, int index)
	{
		newComponentIDs[index] = C::UUID;
	}
}