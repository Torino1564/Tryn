#include "Node.h"

namespace tryn::gfx
{
	Node::Node(int id, std::string_view name, std::vector<std::shared_ptr<Mesh>> pMeshes, glm::mat4 transform)
		:
		id(id),name(name.data()),pMeshes(std::move(pMeshes))
	{
		parentTransform = std::move(transform);
		selfTransform = glm::identity<glm::mat4>();
	}
}

