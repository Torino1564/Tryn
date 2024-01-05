#include "Node.h"
#include <ranges>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/gfx/Model/InstancedModel.h>

namespace tryn::gfx
{
	Node::Node(int id, std::string_view name, std::vector<Mesh*> pMeshes, glm::mat4 transform)
		:
		id(id), name(name.data()), pMeshes(std::move(pMeshes))
	{
		this->transform = std::move(transform);
		this->appliedTransform = glm::identity<glm::mat4>();
	}
	void Node::Submit(IGraphics& gfx, glm::mat4 accumulatedTransform)
	{
		const auto finalTransform = appliedTransform * transform * accumulatedTransform;
		for (auto mesh : pMeshes)
		{
			mesh->Submit(gfx, finalTransform);
		}
		for (auto& child : children)
		{
			child.Submit(gfx, finalTransform);
		}
	}
	void Node::Submit(IGraphics& gfx, std::span<const glm::mat4> accumulatedTransforms, InstancedModelParent& parent)
	{
		auto finalTransforms = mem::ArenaAllocator<>::GP().MakeNewArray<glm::mat4>(accumulatedTransforms.size());
		for (auto [i, accumulatedTransform] : std::ranges::views::enumerate(accumulatedTransforms))
		{
			finalTransforms[i] = appliedTransform * transform * accumulatedTransform;
		}
		for (auto mesh : pMeshes)
		{
			mesh->Submit(gfx, {finalTransforms}, parent);
		}
		for (auto& child : children)
		{
			child.Submit(gfx, { finalTransforms }, parent);
		}
	}
	void Node::AddChild(Node child)
	{
		children.push_back(std::move(child));
	}
}