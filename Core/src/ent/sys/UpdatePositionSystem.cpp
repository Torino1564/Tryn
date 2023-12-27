#include "UpdatePositionSystem.h"
#include <Core/src/ent/Component/ComponentManager.h>

namespace tryn::ent::sys
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
			positionArray[i].position.x += velocityArray[i].velocity.x * dt;
			positionArray[i].position.y += velocityArray[i].velocity.y * dt;
			positionArray[i].position.z += velocityArray[i].velocity.z * dt;
		}
	}
}