#include "EcsClass.h"
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/sys/SystemManager.h>

#include "Core/src/app/App.h"
#include "Core/src/gfx/ImguiManager.h"

namespace tryn::ecs
{
	ECS::ECS(const app::App* pApp)
	:
		pApp(pApp), pComponentManager(std::make_unique<ComponentManager>(this)), pArchetypeManager(std::make_unique<ArchetypeManager>(this)), pSystemManager(std::make_unique<SystemManager>(this))
	{}

	ECS::~ECS() = default;

	void ECS::WipeAllocator()
	{
		allocator.Wipe();
	}

	const gfx::IGraphics& ECS::Gfx() const
	{
		return pApp->Gfx();
	}

	const mem::ArenaAllocator<>& ECS::GetAllocator() const
	{
		return allocator;
	}

	mem::ArenaAllocator<>& ECS::GetAllocator()
	{
		return allocator;
	}

	const ComponentManager& ECS::GetComponentManager() const
	{
		return *pComponentManager;
	}

	ComponentManager& ECS::GetComponentManager()
	{
		return *pComponentManager;
	}

	const ArchetypeManager& ECS::GetArchetypeManager() const
	{
		return *pArchetypeManager;
	}

	ArchetypeManager& ECS::GetArchetypeManager()
	{
		return *pArchetypeManager;
	}

	const SystemManager& ECS::GetSystemManager() const
	{
		return *pSystemManager;
	}

	SystemManager& ECS::GetSystemManager()
	{
		return *pSystemManager;
	}

	void ECS::ExecuteSystems() const
	{
		pSystemManager->ExecuteSystems();
	}

	void ECS::ShowDebugInfo() const
	{
		const auto& componentManager = *pComponentManager;
		const auto& archetypeManager = *pArchetypeManager;
		const auto& systemManager = *pSystemManager;

		ImGui::Begin("ECS Information");
		{
			// Print Registered Components:
			if (ImGui::TreeNode("Components"))
			{
				if (ImGui::BeginTable("componentTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH))
				{
					ImGui::TableSetupColumn("UUID");
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Inspect");
					ImGui::TableHeadersRow();

					for (const auto& [uuid, wrapper] : componentManager.componentWrappers)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(uuid).c_str());

						ImGui::TableNextColumn();
						ImGui::Text(wrapper.Name().data());

						ImGui::TableNextColumn();
						ImGui::Button(std::format("Inspect##{}", uuid).c_str());
					}
					ImGui::EndTable();
				}

				ImGui::TreePop();
			}

			// Print Archetypes
			if (ImGui::TreeNode("Archetypes"))
			{
				if (ImGui::BeginTable("Archetype Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH))
				{
					ImGui::TableSetupColumn("UUID");
					ImGui::TableSetupColumn("Components");
					ImGui::TableSetupColumn("Inspect");
					ImGui::TableHeadersRow();

					for (const auto& archetype : archetypeManager.archetypeBuffer)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(archetype.GetUUID()).c_str());

						ImGui::TableNextColumn();
						for (auto& componentUUID : archetype.components)
						{
							ImGui::Text(componentManager.componentWrappers.at(componentUUID).Name().data());
						}

						ImGui::TableNextColumn();
						const auto& id = std::format("Archetype {} Inspect", archetype.GetUUID()).c_str();
						if (ImGui::Button(std::format("Inspect##{}", archetype.GetUUID()).c_str()))
						{
							const auto previousValue = ImGui::GetStateStorage()->GetBool(ImGui::GetID(id));
							ImGui::GetStateStorage()->SetBool(ImGui::GetID(id), !previousValue);
						}
						if (ImGui::Begin(std::format("Archetype {} Inspect", archetype.GetUUID()).c_str(), ImGui::GetStateStorage()->GetBoolRef(ImGui::GetID(id))));
						{
							if (ImGui::BeginTable(std::format("ArchetypeEntityTable##{}", archetype.GetUUID()).c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH))
							{
								ImGui::TableSetupColumn("ID");
								ImGui::TableSetupColumn("Components");
								ImGui::TableHeadersRow();

								for (uint32_t i = 0; i < archetype.upperLimit; i++)
								{
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);

									ImGui::Text(std::to_string(i).c_str());

									ImGui::TableNextColumn();
									if (ImGui::TreeNode(std::format("Archetype{}EntityComponentList##{}", archetype.GetUUID(), i).c_str()))
									{
										for (const auto uuid : archetype.components)
										{
											ImGui::Text(componentManager.componentWrappers.at(uuid).Name().data());
										}
										ImGui::TreePop();
									}

								}
								ImGui::EndTable();
							}

						}
						ImGui::End();
					}
					ImGui::EndTable();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Systems"))
			{
				if (ImGui::BeginTable("System Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH))
				{
					ImGui::TableSetupColumn("UUID");
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Inspect");
					ImGui::TableHeadersRow();

					for (const auto& pSystem : systemManager.graph.pSystems)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(pSystem->ID()).c_str());

						ImGui::TableNextColumn();
						ImGui::Text(pSystem->Name().data());

						ImGui::TableNextColumn();
						ImGui::Button(std::format("Inspect##{}", pSystem->ID()).c_str());
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}
