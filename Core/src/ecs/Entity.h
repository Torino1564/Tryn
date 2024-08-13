#pragma once
#include <memory>
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/Archetype.h>
#include <array>
#include "EntityID.h"
#include <Core/src/ser/StreamIO.h>

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
		struct Serializer : public tryn::ser::Serializer<Entity>
		{
			template <typename MapElement, ValidComponent C>
			struct SerializeWriteComponentField
			{
				void operator()(const tryn::ser::StreamWriter& streamWriter, const EntityID entityID, const bool binary = true, const std::string& name = "")
				{
					auto data = ECS::Get().archetypeManager.GetArchetype(entityID.archetype)->GetComponentData<C>();
					
					using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
					ByteOffsetFunc_t byteOffsetFunc;

					auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityID.ID - 1]) + byteOffsetFunc());

					streamWriter.Serialize(*pData, binary, name);
				}
			};
			template <typename MapElement, ValidComponent C>
			struct SerializeReadComponentField
			{
				template <typename Data = void>
				void operator()(const tryn::ser::StreamReader& streamReader, const EntityID entityID, const bool binary = true, const Data* pExtraData = nullptr)
				{
					//static_assert(HasGfxPointer<Data> && pExtraData != nullptr, "The SerializeReadComponentField functor requires extra data of type tryn::gfx::IGraphics*!");

					auto data = ECS::Get().archetypeManager.GetArchetype(entityID.archetype)->GetComponentData<C>();
					
					using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
					ByteOffsetFunc_t byteOffsetFunc;

					auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityID.ID - 1]) + byteOffsetFunc());

					streamReader.ReadSerialized(*pData, binary, pExtraData);
				}
			};

			static void Write(const tryn::ser::StreamWriter& streamWriter, const Entity& data, const bool binary = true, const std::string& name = "")
			{
				// Name
				streamWriter.Serialize(data.name, binary, name);

				// Archetype
				streamWriter.Serialize(data.pArchetype, binary, name);

				// Component Data. By default, components are serialized in ascending order by their UUID
				auto sortedVec = data.pArchetype->componentUUIDs;
				std::ranges::sort(sortedVec);
				for (auto componentUUID : sortedVec)
				{
					ComponentManager::IterateComponentMembers<SerializeWriteComponentField>(componentUUID, streamWriter, data.UUID, binary, name);
				}

			}

			template <typename Data = void>
			static Entity Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
			{
				Entity newEntity;

				// Name
				newEntity.name = streamReader.ReadSerialized<std::string>(binary, pExtraData);

				// Archetype & UUID
				newEntity.pArchetype = streamReader.ReadSerialized<Archetype*>(binary, pExtraData);
				newEntity.UUID = newEntity.pArchetype->ResolveEntityUUID();

				auto sortedVec = newEntity.pArchetype->componentUUIDs;
				std::ranges::sort(sortedVec);

				// Component Data. Again, sorted in ascending order

				for (auto componentUUID : sortedVec)
				{
					ComponentManager::IterateComponentMembers<SerializeReadComponentField>(componentUUID, streamReader, newEntity.UUID, binary, pExtraData);
				}

				return newEntity;
			}
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

	template <typename MapElement, ValidComponent C>
	struct PrintImGuiMemberVariable
	{
		void operator()(EntityID entityUUID)
		{
			auto pArchetype = ECS::Get().archetypeManager.GetArchetype(entityUUID.archetype);
			auto data = pArchetype->GetComponentData<C>();

			using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
			ByteOffsetFunc_t byteOffsetFunc;

			auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityUUID.ID - 1]) + byteOffsetFunc());

			cmp::ImGuiPrintElement<MapElement>::Print(pData);
		}
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

	inline void Entity::SpawnControlWindow()
	{
		if (ImGui::Begin(std::format("[{}] - Entity properties", name).c_str()))
		{
			ImGui::Text(std::format("Entity UUID: {}:{}", UUID.archetype, UUID.ID).c_str());
			ImGui::Text("Components:");
			auto& componentIndices = pArchetype->components;
			auto& sortedComponentUUIDs = pArchetype->sortedComponentUUIDs;
			if (ImGui::BeginCombo("Components", "Select a component"))
			{
				if (selectedComponents.size() != ComponentManager::GetComponentCount())
				{
					selectedComponents.resize(ComponentManager::GetComponentCount(), false);
				}
				for (auto componentIndex : componentIndices)
				{
					ImGui::Selectable(GetComponentInfo<ComponentInfo::Name>(ComponentManager::ComponentVector()[componentIndex]), reinterpret_cast<bool*>(&selectedComponents[componentIndex]));
				}
				ImGui::EndCombo();
			}
			for (auto [index, selected] : std::ranges::enumerate_view(selectedComponents))
			{
				if (!selected)
				{
					continue;
				}
				ComponentManager::IterateComponentMembers<PrintImGuiMemberVariable>(sortedComponentUUIDs[index], this->UUID);
			}
			ImGui::End();
		}
	}

}