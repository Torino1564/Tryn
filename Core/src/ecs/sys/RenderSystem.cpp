#include "RenderSystem.h"
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tryn::ecs::sys
{
	void RenderSystem::Execute()
	{
		// Request data

		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::TransformComponent>,
			ReadOnly<cmp::ActiveComponent>,
			WriteOnly<cmp::ModelComponent>>();

		// Reset data arrays

		transformArray.Clear();
		modelArray.Clear();

		// Fill data arrays

		for (auto& queriedData : data)
		{
			transformArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
			modelArray.PushBack(std::get<std::span<cmp::ModelComponent::SubresourceData>>(queriedData));
			activeArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
		}

		// Kernel

		for (auto i = 0; i < modelArray.Size(); i++)
		{
			if (activeArray[i].active)
			{
				modelArray[i].pModel->Submit(transformArray[i].transform);
			}
		}

	}
}

