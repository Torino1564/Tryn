#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <glm/mat4x4.hpp>
#include <Core/src/ser/Serializer.h>

#include "Core/src/ser/StreamIO.h"

namespace tryn::gfx::ani
{
	class Animation;

	struct BoneWeight
	{
		uint32_t vertexID;
		float weight;
	};

	struct Bone
	{
	public:
		Bone(std::string name, uint16_t ID, uint16_t parentID, glm::mat4 inverseBP);
		Bone(std::string name, uint16_t ID, uint16_t parentID = 0);
		void AddWeight(uint32_t vertexID, float weight);

		std::string name;
		uint16_t ID;
		uint16_t parentID;
		glm::mat4 inverseBP;

		std::vector<BoneWeight> boneWeights;
	};

	class Skeleton
	{
	public:
		uint32_t NextID() const;
		std::vector<Bone> bones;
	};

	struct AnimationSkeletonInterface
	{
		AnimationSkeletonInterface(Skeleton& skeleton, Animation& animation);

		Skeleton* pSkeleton;
		Animation* pAnimation;

		using AnimationNodeIndex = uint32_t;
		using BoneIndex = uint32_t;

		std::vector<std::pair<AnimationNodeIndex, BoneIndex>> indexPairs;
	};
}

namespace tryn::ser
{
	// TODO: Finish AnimationSkeletonInterface* serializer
	void Serialize(StreamIO& streamWriter, gfx::ani::AnimationSkeletonInterface* pData, bool binary = true, const std::string& name = "");
}
