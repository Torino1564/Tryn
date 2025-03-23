#include "TrynPCH.h"
#include "AnimationSystem.h"
#include <Core/src/gfx/Animation/Animation.h>
#include "Core/third/glm/gtc/quaternion.hpp"
#include <Core/src/ecs/Archetype.h>
#include "TransformSystem.h"

namespace tryn::ecs
{
	AnimationSystem::AnimationSystem(const SystemGraph& pGraph): SystemImpl(pGraph)
	{}

	void AnimationSystem::InitDependencies(System* self)
	{
		self->AddDependency<TransformSystem>();
	}

	void AnimationSystem::Execute()
	{
		// update state

		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(now - previous).count()) * 10e-6;
		previous = now;

		mem::ArenaAllocator<>& allocator = pEcs->GetAllocator();

		// declare the data

		auto data = pEcs->GetArchetypeManager().GetComponentGroups<
			ReadWrite<ActiveComponent>,
			ReadWrite<BoneTransformsComponent>,
			ReadWrite<TransformComponent>,
			ReadWrite<AnimatedComponent>>();

		// clear the arrays

		animatedArray.Clear();
		boneTransformsArray.Clear();
		activeArray.Clear();
		transformsArray.Clear();

		// fill the arrays

		for (auto& queriedData : data)
		{
			activeArray.PushBack(std::get<std::span<ActiveComponent>>(queriedData));
			animatedArray.PushBack(std::get<std::span<AnimatedComponent>>(queriedData));
			boneTransformsArray.PushBack(std::get<std::span<BoneTransformsComponent>>(queriedData));
			transformsArray.PushBack(std::get<std::span<TransformComponent>>(queriedData));
		}

		// kernel
		for (auto i = 0; i < activeArray.Size(); i++)
		{
			if (!activeArray[i].active)
				continue;
			auto time = animatedArray[i].time + deltaTime;
			animatedArray[i].time = time;
			const auto pSkAnInterface = animatedArray[i].pAnimationSkeletonInterface;

			if (!pSkAnInterface)
				continue;

			const auto& skAnInterface = *pSkAnInterface;

			double realTimePoint = fmod(time, skAnInterface.pAnimation->durationInTicks / skAnInterface.pAnimation->ticksPerSecond );
			double timePoint = realTimePoint * skAnInterface.pAnimation->ticksPerSecond;

			boneTransformsArray[i].transforms = allocator.MakeNewArray<glm::mat4>(skAnInterface.pSkeleton->bones.size());
			auto localTransforms = allocator.MakeNewArray<glm::mat4>(skAnInterface.pSkeleton->bones.size());
			auto& transformArray = boneTransformsArray[i].transforms;

			for (int j = 0; j < transformArray.size(); j++)
			{
				transformArray[j] = glm::identity<glm::mat4>();
				localTransforms[j] = glm::identity<glm::mat4>();
			}

			for (const auto [arrayIndex, boneIndex] : skAnInterface.indexPairs)
			{
				const auto& position = skAnInterface.pAnimation->nodes[arrayIndex].GetPositionVectorKey(timePoint);
				const auto& scale = skAnInterface.pAnimation->nodes[arrayIndex].GetScaleVectorKey(timePoint);
				const auto& rotation = skAnInterface.pAnimation->nodes[arrayIndex].GetRotationVectorKey(timePoint);

				const auto translationMatrix = glm::translate(glm::mat4(1.0f), position);
				const auto scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
				const auto rotationMatrix = glm::mat4_cast(rotation);

				const auto transformMatrix = transformsArray[i].transform;

				glm::mat4 localTransform = translationMatrix * rotationMatrix * scaleMatrix;

				if (boneIndex == 0)
					localTransforms[boneIndex] = localTransform;
				else
					localTransforms[boneIndex] = localTransforms[skAnInterface.pSkeleton->bones[boneIndex].parentID] * localTransform;

				const auto& inverseBP = skAnInterface.pSkeleton->bones[boneIndex].inverseBP;
				transformArray[boneIndex] =  transpose(transformMatrix * localTransforms[boneIndex] * inverseBP * inverse(transformMatrix));
			}
		}
	}
	void AnimationSystem::Init()
	{
		previous = std::chrono::high_resolution_clock::now();
	}
}
