#include "Node.h"

namespace tryn::gfx
{
	Node::Node(int id, std::string_view name, std::vector<Mesh*> pMeshes, glm::mat4 transform)
		:
		id(id), name(name.data()), pMeshes(std::move(pMeshes))
	{
		this->transform = std::move(transform);
		this->appliedTransform = glm::identity<glm::mat4>();
	}
	void Node::Draw(IGraphics& gfx, glm::mat4 accumulatedTransform)
	{
		const auto finalTransform = glm::transpose(accumulatedTransform * transform * appliedTransform);
		for (auto& mesh : pMeshes)
		{
			mesh->Draw(gfx,finalTransform);
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