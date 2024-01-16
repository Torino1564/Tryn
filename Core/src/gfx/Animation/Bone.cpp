#include "Bone.h"

namespace tryn::gfx::ani
	{
	Bone::Bone(std::string name, uint16_t ID, uint16_t parentID, glm::mat4 inverseBP)
		:
		name(std::move(name)), ID(ID), parentID(ID), inverseBP(std::move(inverseBP))
	{}

	Bone::Bone(std::string name, uint16_t ID, uint16_t parentID)
		:
		name(std::move(name)), ID(ID), parentID(parentID), inverseBP({})
	{
	}

	void Bone::AddWeight(uint32_t vertexID, float weight)
	{
		boneWeights.push_back({ vertexID, weight });
	}

	uint32_t Skeleton::NextID() const
	{
		return bones.size();
	}

}
