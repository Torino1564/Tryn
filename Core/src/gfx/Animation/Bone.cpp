#include "TrynPCH.h"
#include "Bone.h"
#include <ranges>
#include "Animation.h"
#include <algorithm>

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

	AnimationSkeletonInterface::AnimationSkeletonInterface(Skeleton& skeleton, Animation& animation)
	{
		pSkeleton = &skeleton;
		pAnimation = &animation;

		for (auto [index, animationNode] : std::ranges::views::enumerate(animation.GetNodes()))
		{
			auto it = std::find_if(skeleton.bones.begin(), skeleton.bones.end(), [&](const ani::Bone& bone)
				{
					return (strcmp(bone.name.c_str(), animationNode.boneName.c_str()) == 0);
				});

			if (it == skeleton.bones.end())
				continue;
			
			indexPairs.push_back({ index, it->ID });
		}
		using Element = decltype(indexPairs)::value_type;
		std::sort(indexPairs.begin(), indexPairs.end(), [&](const Element& e1, const Element& e2) {
			return e1.second < e2.second;
			});
	}

}

void tryn::ser::Serialize(StreamIO& streamWriter, gfx::ani::AnimationSkeletonInterface* pData, const bool binary,
	const std::string& name)
{
}
