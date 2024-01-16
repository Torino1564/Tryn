#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <Core/third/glm/mat4x4.hpp>

namespace tryn::gfx::ani
{
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
}