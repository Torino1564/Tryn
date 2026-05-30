
#include "TransformSystem.h"
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Core/src/ecs/sys/UpdatePositionSystem.h>
#include <Core/src/ecs/Archetype.h>

#include "Core/src/gfx/CoreGraphics.h"

namespace tryn::ecs
{
	TransformSystem::TransformSystem(const SystemGraph& pGraph): SystemImpl(pGraph)
	{}

	void TransformSystem::InitDependencies(System* self)
	{
		self->AddDependency<UpdatePositionSystem>();
	}

	void TransformSystem::Execute()
	{
		// Request Data
		auto data = pEcs->GetArchetypeManager().GetComponentGroups<
			ReadOnly<PositionComponent>,
			ReadOnly<ScaleComponent>,
			ReadOnly<RotationComponent>,
			ReadOnly<ActiveComponent>,
			WriteOnly<TransformComponent>>();
		
		// Reset arrays
		
		positionArray.Clear();
		scaleArray.Clear();
		rotationArray.Clear();
		transformArray.Clear();
		activeArray.Clear();
		
		// Fill data arrays

		for (auto& queriedData : data)
		{
			positionArray.PushBack(std::get<std::span<PositionComponent>>(queriedData));
			scaleArray.PushBack(std::get<std::span<ScaleComponent>>(queriedData));
			rotationArray.PushBack(std::get<std::span<RotationComponent>>(queriedData));
			transformArray.PushBack(std::get<std::span<TransformComponent>>(queriedData));
			activeArray.PushBack(std::get<std::span<ActiveComponent>>(queriedData));
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
				transformArray[i].transform = translationMatrix * rotationMatrix * scalingMatrix;
			}
		}
	}
}
