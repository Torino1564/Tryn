
#include "UpdateVelocitySystem.h"
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/Archetype.h>

namespace tryn::ecs
{
	UpdateVelocitySystem::UpdateVelocitySystem(const SystemGraph& pGraph) : SystemImpl(pGraph)
	{}

	void UpdateVelocitySystem::Execute()
	{
		// Check stopwatch
		const auto dt = sw.StopAndRestart();

		// Request data

		auto data = pEcs->GetArchetypeManager().GetComponentGroups<
			ReadOnly<AccelerationComponent>,
			ReadWrite<VelocityComponent>>();

		// Clear data arrays

		velocityArray.Clear();
		accelerationArray.Clear();

		// Fill data arrays

		for (auto& entry : data)
		{
			velocityArray.PushBack(std::get<std::span<VelocityComponent>>(entry));
			accelerationArray.PushBack(std::get<std::span<AccelerationComponent>>(entry));
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