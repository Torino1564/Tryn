#include "TrynPCH.h"
#include "Entity.h"
#include <Core/src/ecs/cmp/Components.h>
#include <ranges>

namespace tryn::ecs
{
	Entity::Entity(std::string name)
		:
		name(std::move(name))
	{

	}

	Entity::~Entity()
	{
		pArchetype->Free(UUID);
	}

	std::span<unsigned int> Entity::GetComponents()
	{
		return std::span(pArchetype->components.begin(), pArchetype->components.size());
	}

	void Entity::Instanciate(std::span<Entity> destination)
	{
		for (auto [instanceNum, ent] : std::ranges::views::enumerate(destination))
		{
			ent.name = name + "_" + std::to_string(instanceNum);
			ent.pArchetype = pArchetype;
			ent.UUID = pArchetype->ResolveEntityUUID();
		}
	}

	void Entity::Serializer::Write(const ser::StreamWriter& streamWriter, const Entity& data, const bool binary,
		const std::string& name)
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
			//ComponentManager::IterateComponentMembers<ecs::SerializeWriteComponentField>(componentUUID, streamWriter, data.UUID, binary, name);
		}

	}

	Entity Entity::Serializer::Read(const ser::StreamReader& streamReader, const bool binary,
		const ser::ExtraDataPack* pExtraData)
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
			//ComponentManager::IterateComponentMembers<ecs::SerializeReadComponentField>(componentUUID, streamReader, newEntity.UUID, binary, pExtraData);
		}

		return newEntity;
	}

	void Entity::SpawnControlWindow()
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
			const auto& views = ComponentManager::ComponentViewVector();
			for (auto [index, selected] : std::ranges::enumerate_view(selectedComponents))
			{
				if (!selected)
				{
					continue;
				}
				views[index].Func<3>(this->UUID);
				//ComponentManager::IterateComponentMembers<PrintImGuiMemberVariable>(sortedComponentUUIDs[index], this->UUID);
			}
			ImGui::End();
		}
	}
}
