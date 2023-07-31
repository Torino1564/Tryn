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
	private:
		std::string name;
		uint32_t id = 0;
		std::vector<Mesh> meshes;
		std::vector<Node> children;
		glm::mat4 transform = {};
		glm::mat4 parentTransform = {};
	};
}
