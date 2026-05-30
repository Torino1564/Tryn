#pragma once
#include <vector>
#include <string>
#include "Key.h"
#include <glm/glm.hpp>
#include <span>
#include "Bone.h"
#include "AnimationState.h"

struct aiAnimation;

namespace tryn::gfx::ani
{
	struct AnimationSkeletonInterface;

	struct AnimationNode;

	class Animation
	{
		friend class AnimationManager;
	public:
		Animation(const std::string& path, uint32_t id, const aiAnimation& anim);
		std::span<const AnimationNode> GetNodes() const;
		double durationInTicks;
		double ticksPerSecond;
		uint32_t ID;
		std::string name;
		std::vector<AnimationNode> nodes;
	};

	struct AnimationNode
	{
		std::string boneName;
		uint32_t numPositionKeys;
		uint32_t numScalingKeys;
		uint32_t numRotationKeys;
		std::vector<VectorKey> positionKeys;
		std::vector<VectorKey> scalingKeys;
		std::vector<QuatKey> rotationKeys;

		const glm::vec3& GetPositionVectorKey(double timePoint) const;
		const glm::vec3& GetScaleVectorKey(double timePoint) const;
		const glm::quat& GetRotationVectorKey(double timePoint) const;
	};
}