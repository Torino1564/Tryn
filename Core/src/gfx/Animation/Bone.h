#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <Core/third/glm/mat4x4.hpp>
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

	template <> struct ser::TypeSerializer<gfx::ani::AnimationSkeletonInterface*>
	{
		static void Write(const StreamWriter& streamWriter, gfx::ani::AnimationSkeletonInterface* const& pData, const bool binary = true, const std::string& name = "")
		{
			streamWriter.Serialize(0u, binary, name);
		}

		template <typename Data = void>
		static gfx::ani::AnimationSkeletonInterface* Read(const StreamReader& streamReader, const bool binary = true, const std::string& name = "", const Data* pExtraData = nullptr)
		{
			return nullptr;
		}

		template <typename Data = void>
		static void Read(const StreamReader& streamReader, gfx::ani::AnimationSkeletonInterface* pData, const bool binary = true, const std::string& name = "", const Data* pExtraData = nullptr)
		{
			pData = nullptr;
		}
	};
}