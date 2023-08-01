#pragma once
#include <memory>
#include <string>
#include <vector>
#include <core/third/glm/glm.hpp>
#include "Mesh.h"

namespace tryn::gfx
{
	class Node
	{
		Node(int id, std::string_view name, std::vector<std::shared_ptr<Mesh>> pMeshes, glm::mat4 transform);
	private:
		std::string name;
		uint32_t id = 0;
		std::vector<Mesh&> pMeshes;
		std::vector<Node> children;
		glm::mat4 selfTransform = {};
		glm::mat4 parentTransform = {};
	};
}
