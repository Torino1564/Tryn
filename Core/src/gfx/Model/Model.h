#pragma once
#include <memory>
#include "Node.h"
#include <Core/src/gfx/IGraphics.h>
#include "Mesh.h"

namespace tryn::gfx
{
	class Model
	{
	public:
		Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = {1.0f,1.0f,1.0f});
	private:
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale);
	private:
		std::string name;
		std::unique_ptr<Node> root;
		std::vector<std::shared_ptr<Mesh>> pMeshes;
	};
}
