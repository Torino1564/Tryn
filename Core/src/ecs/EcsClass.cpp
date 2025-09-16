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
				if (ImGui::BeginTable("Archetype Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH))
				{
					ImGui::TableSetupColumn("UUID");
					ImGui::TableSetupColumn("Name");
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
						ImGui::Button(std::format("Inspect##{}", archetype.GetUUID()).c_str());
					}
					ImGui::EndTable();
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}
