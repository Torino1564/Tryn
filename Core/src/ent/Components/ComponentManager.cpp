#include "ComponentManager.h"
#include <Core/third/imgui/imgui.h>

void tryn::ent::ComponentManager::Update(double dt)
{
	for (auto& pComponent : componentPtrs)
	{
		pComponent->OnUpdate(dt);
	}
}

void tryn::ent::ComponentManager::AddComponent(std::unique_ptr<Component>&& pNewComponent)
{
	componentPtrs.emplace_back(std::move(pNewComponent));
}

void tryn::ent::ComponentManager::ComponentTree()
{
	ImGui::TreeNode("Components");
	for (auto& pComponent : componentPtrs)
	{
		pComponent->Controls();
	}
}
