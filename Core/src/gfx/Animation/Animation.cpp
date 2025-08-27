
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
			
			node.positionKeys.reserve(node.numPositionKeys);
			node.rotationKeys.reserve(node.numRotationKeys);
			node.scalingKeys.reserve(node.numScalingKeys);

			for (unsigned int j = 0; j < node.numScalingKeys; j++)
			{
				{
					auto& key = channel.mScalingKeys[j];
					const auto q = key.mValue;
					node.scalingKeys.emplace_back(VectorKey(key.mTime, {q.x, q.y, q.z}));
				}
				{
					auto& key = channel.mPositionKeys[j];
					const auto q = key.mValue;
					node.positionKeys.emplace_back(VectorKey(key.mTime, { q.x, q.y, q.z }));
				}
				{
					auto& key = channel.mRotationKeys[j];
					const auto q = key.mValue;
					node.rotationKeys.emplace_back(QuatKey(key.mTime, {q.w, q.x, q.y, q.z}));
				}
			}
		}
	}
	std::span<const AnimationNode> Animation::GetNodes() const
	{
		return { nodes };
	}
	const glm::vec3& AnimationNode::GetPositionVectorKey(const double timePoint) const
	{
		uint32_t newKey = std::floor(timePoint);
		do
		{
			if (newKey == numPositionKeys - 1)
			{
				newKey = 0;
			}
			if (timePoint > positionKeys[newKey].time &&
				timePoint < positionKeys[newKey + 1].time)
			{
				return positionKeys[newKey].value;
			}
			newKey++;

		} while (true);
	}

	const glm::quat& AnimationNode::GetRotationVectorKey(const double timePoint) const
	{
		uint32_t newKey = std::floor(timePoint);
		do
		{
			if (newKey == numPositionKeys - 1)
			{
				newKey = 0;
			}
			if (timePoint > rotationKeys[newKey].time &&
				timePoint < rotationKeys[newKey + 1].time)
			{
				return rotationKeys[newKey].value;
			}
			newKey++;

		} while (true);
	}

	const glm::vec3& AnimationNode::GetScaleVectorKey(const double timePoint) const
	{
		uint32_t newKey = std::floor(timePoint);
		do
		{
			if (newKey == numPositionKeys - 1)
			{
				newKey = 0;
			}
			if (timePoint > scalingKeys[newKey].time &&
				timePoint < scalingKeys[newKey + 1].time)
			{
				return scalingKeys[newKey].value;
			}
			newKey++;

		} while (true);
	}
}
