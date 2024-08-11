#include "TrynPCH.h"
#include "Node.h"
#include <ranges>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include "Core/third/glm/ext/matrix_transform.hpp"
#include <Core/src/gfx/Animation/BonedMesh.h>
#include "Mesh.h"

namespace tryn::gfx
{
	Node::Node(int id, std::string_view name, std::vector<uint16_t> meshIds, glm::mat4 transform, bool isSkeleton)
		:
		id(id), name(name.data()), meshIds(std::move(meshIds)), isSkeleton(isSkeleton)
	{
		this->transform = std::move(transform);
		this->appliedTransform = glm::identity<glm::mat4>();
	}
	void Node::Submit(const IGraphics& gfx, glm::mat4 accumulatedTransform)
	{
		const auto finalTransform = appliedTransform * transform * accumulatedTransform;
		for (auto id : meshIds)
		{
			meshes[id]->Submit(gfx, finalTransform);
		}
		for (auto& child : children)
		{
			child.Submit(gfx, finalTransform);
		}
	}
	void Node::Submit(const IGraphics& gfx, std::span<const glm::mat4> accumulatedTransforms, InstancedModelParent& parent)
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
		for (auto& child : children)
		{
			child.Submit(gfx, { finalTransforms }, parent);
		}
	}
	void Node::Submit(const IGraphics& gfx, glm::mat4 accumulatedTransform, std::span<const glm::mat4> boneTransforms)
	{
		// get main mesh
		ani::BonedMesh* pBonedMesh = reinterpret_cast<ani::BonedMesh*>(meshes[0].get());
		pBonedMesh->Submit(gfx, accumulatedTransform, boneTransforms);
	}
	void Node::AddChild(Node child)
	{
		children.push_back(std::move(child));
	}
	void Node::SetMeshSpan(std::span<std::shared_ptr<Mesh>> meshSpan)
	{
		meshes = meshSpan;
	}
	std::vector<Node>& Node::GetChildren()
	{
		return children;
	}
}
