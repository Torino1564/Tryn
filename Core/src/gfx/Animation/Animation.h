#pragma once
#include <vector>
#include <string>
#include "Key.h"
#include <Core/third/glm/glm.hpp>
#include <assimp/anim.h>
#include <span>
#include "Bone.h"
#include "AnimationState.h"

namespace tryn::gfx::ani
{
	struct AnimationSkeletonInterface;

	struct AnimationNode;

	class Animation
	{
		friend class AnimationManager;
	public:
		Animation(const std::string& path, const uint32_t id, aiAnimation& anim);
		void Animate(AnimationSkeletonInterface& animInterface, double timePoint, std::span<glm::mat4> boneTransforms);
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

		const glm::vec3& GetPositionVectorKey(uint32_t& previousKey, const double timePoint);
		const glm::vec3& GetScaleVectorKey(uint32_t& previousKey, const double timePoint);
		const glm::quat& GetRotationVectorKey( uint32_t& previousKey, const double timePoint);
	};
}