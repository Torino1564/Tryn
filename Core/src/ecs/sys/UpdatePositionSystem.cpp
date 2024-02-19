#include "UpdatePositionSystem.h"
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tryn::ecs::sys
{
	void UpdatePositionSystem::Execute()
	{
		// Check stopwatch
		const auto dt = sw.StopAndRestart();

		// Request data

		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::VelocityComponent>,
			ReadWrite<cmp::PositionComponent>>();

		// Clear data arrays

		positionArray.Clear();
		velocityArray.Clear();

		// Fill data arrays

		for (auto& entry : data)
		{
			positionArray.PushBack(std::get<std::span<cmp::PositionComponent::SubresourceData>>(entry));
			velocityArray.PushBack(std::get<std::span<cmp::VelocityComponent::SubresourceData>>(entry));
		}

		// Kernel

		for (auto i = 0; i < positionArray.Size(); i++)
		{
			positionArray[i].position.x += (float)velocityArray[i].velocity.x * (float)dt;
			positionArray[i].position.y += (float)velocityArray[i].velocity.y * (float)dt;
			positionArray[i].position.z += (float)velocityArray[i].velocity.z * (float)dt;
		}
	}
}