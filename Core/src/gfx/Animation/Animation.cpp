#include "Animation.h"
#include <Core/third/glm/ext/quaternion_common.hpp>
#include <Core/third/glm/ext/quaternion_float.hpp>
#include "Bone.h"
namespace tryn::gfx::ani
{
	Animation::Animation(const std::string& path, const uint32_t id, aiAnimation& anim)
		: ID(id)
	{
		name = path + anim.mName.C_Str();
		durationInTicks = anim.mDuration;
		ticksPerSecond = anim.mTicksPerSecond;

		nodes.reserve(anim.mNumChannels);;
		for (auto i = 0; i < anim.mNumChannels; i++)
		{
			auto& channel = *anim.mChannels[i];
			auto& node = nodes.emplace_back();

			node.boneName = channel.mNodeName.C_Str();
			node.numPositionKeys = channel.mNumPositionKeys;
			node.numScalingKeys = channel.mNumScalingKeys;
			node.numRotationKeys = channel.mNumRotationKeys;
			
			node.scalingKeys.reserve(node.numScalingKeys);
			for (auto i = 0; i < node.numScalingKeys; i++)
			{
				auto& key = channel.mScalingKeys[i];
				node.scalingKeys.push_back(VectorKey(key.mTime, reinterpret_cast<glm::vec3*>(&key.mValue)));
			}

			node.positionKeys.reserve(node.numPositionKeys);
			for (auto i = 0; i < node.numScalingKeys; i++)
			{
				auto& key = channel.mPositionKeys[i];
				node.positionKeys.push_back(VectorKey(key.mTime, reinterpret_cast<glm::vec3*>(&key.mValue)));
			}

			node.rotationKeys.reserve(node.numRotationKeys);
			for (auto i = 0; i < node.numRotationKeys; i++)
			{
				auto& key = channel.mRotationKeys[i];
				node.rotationKeys.push_back(QuatKey(key.mTime, reinterpret_cast<glm::quat*>(&key.mValue)));
			}
		}
	}
	void Animation::Animate(AnimationSkeletonInterface& animInterface, double timePoint, std::span<glm::mat4> boneTransforms)
	{

	}
	std::span<const AnimationNode> Animation::GetNodes() const
	{
		return { nodes };
	}
	const glm::vec3& AnimationNode::GetPositionVectorKey( uint32_t& previousKey, const double timePoint)
	{
		uint32_t newKey = 0;
		uint32_t idx = previousKey;
		do
		{
			if (idx == numPositionKeys - 1)
			{
				idx = 0;
				newKey = idx;
				previousKey = newKey;
				return positionKeys[idx].value;
			}
			if (timePoint > positionKeys[idx].time &&
				timePoint < positionKeys[idx + 1].time)
			{
				newKey = idx;
				previousKey = newKey;
				return positionKeys[idx].value;
			}
			idx++;

		} while (previousKey == newKey);
	}

	const glm::quat& AnimationNode::GetRotationVectorKey(uint32_t& previousKey, const double timePoint)
	{
		uint32_t idx = previousKey;
		do
		{
			if (idx == numRotationKeys - 1)
			{
				idx = 0;
				previousKey = idx;
				return rotationKeys[idx].value;
			}
			if (timePoint > rotationKeys[idx].time &&
				timePoint < rotationKeys[idx + 1].time)
			{
				previousKey = idx;
				return rotationKeys[idx].value;
			}

		} while (previousKey != idx);
	}

	const glm::vec3& AnimationNode::GetScaleVectorKey(uint32_t& previousKey, const double timePoint)
	{
		uint32_t idx = previousKey;
		do
		{
			if (idx == numScalingKeys - 1)
			{
				idx = 0;
				previousKey = idx;
				return scalingKeys[idx].value;
			}
			if (timePoint > scalingKeys[idx].time &&
				timePoint < scalingKeys[idx + 1].time)
			{
				previousKey = idx;
				return scalingKeys[idx].value;
			}

		} while (previousKey != idx);
	}
}
