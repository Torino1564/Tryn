#include "RenderSystem.h"
#include <Core/src/ent/Component/ComponentManager.h>

namespace tryn::ent::sys
{
	void RenderSystem::Execute()
	{
		// Prepare arrays

		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::PositionComponent>,
			WriteOnly<cmp::ModelComponent>>();

		position.Clear();
		model.Clear();

		for (auto& queriedData : data)
		{
			position.PushBack(std::get<std::span<cmp::PositionComponent::SubresourceData>>(queriedData));
			model.PushBack(std::get<std::span<cmp::ModelComponent::SubresourceData>>(queriedData));
		}


	}
}

