#include "TrynPCH.h"
#include "Animation.h"
#include <Core/third/glm/ext/quaternion_common.hpp>
#include <Core/third/glm/ext/quaternion_float.hpp>
#include <assimp/anim.h>


namespace tryn::gfx::ani
{
	Animation::Animation(const std::string& path, const uint32_t id, const aiAnimation& anim)
		: ID(id)
	{
		name = path + anim.mName.C_Str();
		durationInTicks = anim.mDuration;
		ticksPerSecond = anim.mTicksPerSecond;

		nodes.reserve(anim.mNumChannels);;
		for (unsigned int i = 0; i < anim.mNumChannels; i++)
		{
			auto& channel = *anim.mChannels[i];
			auto& node = nodes.emplace_back();

			node.boneName = channel.mNodeName.C_Str();
			node.numPositionKeys = channel.mNumPositionKeys;
			node.numScalingKeys = channel.mNumScalingKeys;
			node.numRotationKeys = channel.mNumRotationKeys;
			
			node.scalingKeys.reserve(node.numScalingKeys);
			for (unsigned int j = 0; j < node.numScalingKeys; j++)
			{
				auto& key = channel.mScalingKeys[j];
				node.scalingKeys.emplace_back(VectorKey(key.mTime, reinterpret_cast<glm::vec3*>(&key.mValue)));
			}

			node.positionKeys.reserve(node.numPositionKeys);
			for (unsigned int j = 0; j < node.numScalingKeys; j++)
			{
				auto& key = channel.mPositionKeys[j];
				node.positionKeys.emplace_back(VectorKey(key.mTime, reinterpret_cast<glm::vec3*>(&key.mValue)));
			}

			node.rotationKeys.reserve(node.numRotationKeys);
			for (unsigned int j = 0; j < node.numRotationKeys; j++)
			{
				auto& key = channel.mRotationKeys[j];
				node.rotationKeys.emplace_back(QuatKey(key.mTime, reinterpret_cast<glm::quat*>(&key.mValue)));
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
	const glm::vec3& AnimationNode::GetPositionVectorKey( uint32_t& previousKey, const double timePoint) const
	{
		static constexpr glm::vec3 fallback = { 0.0f,0.0f,0.0f };
		uint32_t newKey = previousKey;
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
		
		return fallback;
	}

	const glm::quat& AnimationNode::GetRotationVectorKey(uint32_t& previousKey, const double timePoint) const
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

	const glm::vec3& AnimationNode::GetScaleVectorKey(uint32_t& previousKey, const double timePoint) const
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
