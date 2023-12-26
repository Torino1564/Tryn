#include "TransformSystem.h"
#include <Core/third/glm/ext.hpp>
#include <Core/third/glm/gtx/euler_angles.hpp>

namespace tryn::ent::sys
{
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
				const auto& position = positionArray[i];
				const auto& rotation = rotationArray[i];
				const auto& scale = scaleArray[i];

				glm::mat4x4 transformMatrix = glm::identity<glm::mat4>();

				transformMatrix = glm::scale(transformMatrix, scale.scale);
				transformMatrix = glm::translate(transformMatrix, position.position);
				const auto rotationMatrix = glm::yawPitchRoll(rotation.yaw, rotation.pitch, rotation.roll);
				transformArray[i].transform = transformMatrix * rotationMatrix;
			}
		}
	}
}
