#pragma once
#include <vector>
#include <string>
#include "Bone.h"
#include "Key.h"

#include <Core/third/glm/glm.hpp>

namespace tryn::gfx::ani
{
	class Animation
	{
	private:
		double durationInTicks;
		double ticksPerSecond;

		std::vector<struct AnimationNode> nodes;
	};

	struct AnimationNode
	{
		std::string boneName;
		uint32_t numPositionKeys;
		uint32_t numScalingKeys;
		uint32_t numRotationKeys;
		std::vector<Key<glm::vec3>> positionKeys;
		std::vector<Key<glm::vec3>> scalingKeys;
		std::vector<Key<glm::quat>> rotationKeys;
	};
}