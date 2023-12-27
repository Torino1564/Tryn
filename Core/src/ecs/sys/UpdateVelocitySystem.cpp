#include "UpdateVelocitySystem.h"
#include <Core/src/ecs/cmp/ComponentManager.h>

namespace tryn::ecs::sys
{
	void UpdateVelocitySystem::Execute()
	{
		// Check stopwatch
		const auto dt = sw.StopAndRestart();

		// Request data

		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::AccelerationComponent>,
			ReadWrite<cmp::VelocityComponent>>();

		// Clear data arrays

		velocityArray.Clear();
		accelerationArray.Clear();

		// Fill data arrays

		for (auto& entry : data)
		{
			velocityArray.PushBack(std::get<std::span<cmp::VelocityComponent::SubresourceData>>(entry));
			accelerationArray.PushBack(std::get<std::span<cmp::AccelerationComponent::SubresourceData>>(entry));
		}

		// Kernel

		for (auto i = 0; i < velocityArray.Size(); i++)
		{
			velocityArray[i].velocity.x += accelerationArray[i].acceleration.x * dt;
			velocityArray[i].velocity.y += accelerationArray[i].acceleration.y * dt;
			velocityArray[i].velocity.z += accelerationArray[i].acceleration.z * dt;
		}
	}
}