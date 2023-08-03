#include "Node.h"

namespace tryn::gfx
{
	Node::Node(int id, std::string_view name, std::vector<Mesh*> pMeshes, glm::mat4 transform)
		:
		id(id), name(name.data()), pMeshes(std::move(pMeshes))
	{
		parentTransform = std::move(transform);
		selfTransform = glm::identity<glm::mat4>();
	}
	void Node::Draw(IGraphics& gfx, glm::mat4 accumulatedTransform)
	{
		const auto finalTransform = accumulatedTransform * selfTransform * parentTransform;
		for (auto& mesh : pMeshes)
		{
			mesh->Draw(gfx);
		}
		for (auto& child : children)
		{
			child.Draw(gfx, finalTransform);
		}
	}
	void Node::AddChild(Node child)
	{
		children.push_back(std::move(child));
	}
}