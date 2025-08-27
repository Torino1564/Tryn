
#include "Node.h"
#include <ranges>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include "Core/third/glm/ext/matrix_transform.hpp"
#include <Core/src/gfx/Animation/BonedMesh.h>
#include "Mesh.h"
#include "Model.h"

namespace tryn::gfx
{
	Node::Node(const int id, const std::string_view name, std::vector<uint16_t> meshIds, glm::mat4 transform, Model* pModel, const bool isSkeleton)
		:
	isSkeleton(isSkeleton),
	name(name.data()),
	id(id),
	meshIds(std::move(meshIds)),
	meshes(pModel->pMeshes),
	pModel(pModel),
	transform(std::move(transform)),
	appliedTransform(glm::identity<glm::mat4>()) {}

	void Node::Submit(const IGraphics& gfx, const glm::mat4& accumulatedTransform) const
	{
		const auto finalTransform = appliedTransform * transform * accumulatedTransform;
		for (auto id : meshIds)
		{
			meshes[id]->Submit(gfx, finalTransform);
		}
		auto& nodes = pModel->nodes;
		for (auto& childId : childrenIds)
		{
			auto& child = nodes[childId];
			child.Submit(gfx, finalTransform);
		}
	}
	void Node::Submit(const IGraphics& gfx, std::span<const glm::mat4> accumulatedTransforms, InstancedModelParent& parent) const
	{
		auto finalTransforms = mem::ArenaAllocator<>::GP().MakeNewArray<glm::mat4>(accumulatedTransforms.size());
		for (auto [i, accumulatedTransform] : std::ranges::views::enumerate(accumulatedTransforms))
		{
			finalTransforms[i] = appliedTransform * transform * accumulatedTransform;
		}
		for (auto id : meshIds)
		{
			meshes[id]->Submit(gfx, {finalTransforms}, parent);
		}
		auto& nodes = pModel->nodes;
		for (auto& childId : childrenIds)
		{
			auto& child = nodes[childId];
			child.Submit(gfx, { finalTransforms }, parent);
		}
	}
	void Node::SubmitBoned(const IGraphics& gfx, const glm::mat4& accumulatedTransform, const std::span<const glm::mat4> boneTransforms) const
	{
		// get main mesh
		meshes[0]->SubmitBoned(gfx, accumulatedTransform, boneTransforms);
	}
	void Node::AddChildId(const uint32_t index)
	{
		childrenIds.push_back(index);
	}
	void Node::SetMeshSpan(const std::span<std::shared_ptr<Mesh>> meshSpan)
	{
		meshes = meshSpan;
	}
	std::vector<uint32_t>& Node::GetChildrenIds()
	{
		return childrenIds;
	}
}
