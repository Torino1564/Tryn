#include "TrynPCH.h"
#include "TransformSystem.h"
#include <Core/third/glm/ext.hpp>
#include <Core/third/glm/gtx/euler_angles.hpp>
#include <Core/src/ecs/sys/UpdatePositionSystem.h>

namespace tryn::ecs::sys
{
	TransformSystem::TransformSystem(const SystemGraph& pGraph): SystemImpl(pGraph)
	{}

	void TransformSystem::InitDependencies(System* self)
	{
		self->AddDependency<sys::UpdatePositionSystem>();
	}

	void TransformSystem::Execute()
	{
		// Request Data
		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::PositionComponent>,
			ReadOnly<cmp::ScaleComponent>,
			ReadOnly<cmp::RotationComponent>,
			ReadOnly<cmp::ActiveComponent>,
			WriteOnly<cmp::TransformComponent>>();
		
		// Reset arrays
		
		positionArray.Clear();
		scaleArray.Clear();
		rotationArray.Clear();
		transformArray.Clear();
		activeArray.Clear();
		
		// Fill data arrays

		for (auto& queriedData : data)
		{
			positionArray.PushBack(std::get<std::span<cmp::PositionComponent::SubresourceData>>(queriedData));
			scaleArray.PushBack(std::get<std::span<cmp::ScaleComponent::SubresourceData>>(queriedData));
			rotationArray.PushBack(std::get<std::span<cmp::RotationComponent::SubresourceData>>(queriedData));
			transformArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
			activeArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
		}

		// Kernel

		for (auto i = 0; i < transformArray.Size(); i++)
		{
			if (activeArray[i].active)
			{
				const auto& [position] = positionArray[i];
				const auto& [yaw, pitch, roll] = rotationArray[i];
				const auto& [scale] = scaleArray[i];

				const auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), scale);
				const auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), position);
				const auto rotationMatrix = glm::yawPitchRoll(yaw, pitch, roll);
				transformArray[i].transform = translationMatrix * scalingMatrix * rotationMatrix;
			}
		}
	}
}
