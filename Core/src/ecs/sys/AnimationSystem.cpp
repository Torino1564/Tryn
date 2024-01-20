#include "AnimationSystem.h"

namespace tryn::ecs::sys
{
	void AnimationSystem::Execute()
	{
		// update state

		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(now - previous).count()) * 10e-6;
		previous = now;

		// declare the data

		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			cmp::ActiveComponent,
			cmp::BoneTransformsComponent,
			cmp::TransformComponent,
			cmp::AnimatedComponent>();

		// clear the arrays

		animatedArray.Clear();
		boneTransformsArray.Clear();
		activeArray.Clear();
		transformsArray.Clear();

		// fill the arrays

		for (auto& queriedData : data)
		{
			activeArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
			animatedArray.PushBack(std::get<std::span<cmp::AnimatedComponent::SubresourceData>>(queriedData));
			boneTransformsArray.PushBack(std::get<std::span<cmp::BoneTransformsComponent::SubresourceData>>(queriedData));
			transformsArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
		}

		// kernel
		for (auto i = 0; i < activeArray.Size(); i++)
		{
			if (!activeArray[i].active)
				continue;
			auto time = animatedArray[i].time + deltaTime;
			animatedArray[i].time = time;
			const auto& skAnInterface = *animatedArray[i].pAnimationSkeletonInterface;

			double realtimePoint = fmod(time, skAnInterface.pAnimation->durationInTicks * skAnInterface.pAnimation->ticksPerSecond );
			double timePoint = realtimePoint / skAnInterface.pAnimation->ticksPerSecond;

			boneTransformsArray[i].transforms = ECS::Get().allocator.MakeNewArray<glm::mat4>(skAnInterface.pSkeleton->bones.size());
			auto localTransforms = ECS::Get().allocator.MakeNewArray<glm::mat4>(skAnInterface.pSkeleton->bones.size());
			auto& transformArray = boneTransformsArray[i].transforms;

			for (int j = 0; j < transformArray.size(); j++)
			{
				transformArray[j] = glm::identity<glm::mat4>();
				localTransforms[j] = glm::identity<glm::mat4>();
			}
			auto& previousKey = animatedArray[i].previousKey;
			for (int j = 0; j < skAnInterface.indexPairs.size(); j++)
			{
				auto arrayIndex = skAnInterface.indexPairs[j].first;
				auto boneIndex = skAnInterface.indexPairs[j].second;
				const auto& position = skAnInterface.pAnimation->nodes[arrayIndex].GetPositionVectorKey(previousKey, timePoint);
				const auto& scale = skAnInterface.pAnimation->nodes[arrayIndex].scalingKeys[previousKey].value;
				const auto& rotation = skAnInterface.pAnimation->nodes[arrayIndex].rotationKeys[previousKey].value;

				const auto translationMatrix = glm::translate(glm::mat4(1.0f), position);
				const auto scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
				const auto rotationMatrix = glm::mat4_cast(rotation);

				const auto transformMatrix = transformsArray[i].transform;

				glm::mat4 animationTransform = translationMatrix * rotationMatrix * scaleMatrix;
				localTransforms[boneIndex] = localTransforms[skAnInterface.pSkeleton->bones[boneIndex].parentID] * animationTransform;

				transformArray[boneIndex] = transformMatrix * localTransforms[boneIndex] * skAnInterface.pSkeleton->bones[boneIndex].inverseBP * glm::inverse(transformMatrix);
			}
		}
	}
	void Init()
	{

	}
}
