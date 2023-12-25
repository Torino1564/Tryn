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
			WriteOnly<cmp::TransformComponent>>();
		// Fill data arrays

		for (auto& queriedData : data)
		{
			positionArray.PushBack(std::get<std::span<cmp::PositionComponent::SubresourceData>>(queriedData));
			scaleArray.PushBack(std::get<std::span<cmp::ScaleComponent::SubresourceData>>(queriedData));
			rotationArray.PushBack(std::get<std::span<cmp::RotationComponent::SubresourceData>>(queriedData));
			transformArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
		}

		// Kernel

		for (auto i = 0; i < transformArray.Size(); i++)
		{
			transformArray[i];
			const auto& position = positionArray[i];
			const auto& rotation = rotationArray[i];
			const auto& scale = scaleArray[i];
			const auto rotationMatrix = glm::yawPitchRoll(rotation.yaw, rotation.pitch, rotation.roll);
			const auto translationMatrix = glm::translate(glm::mat4(1.0f), position.position);
			transformArray[i].transform = translationMatrix * rotationMatrix;
		}
	}
}
