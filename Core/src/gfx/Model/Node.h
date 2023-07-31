#pragma once
#include <memory>
#include <string>
#include <vector>
#include <core/third/glm/glm.hpp>

namespace tryn::gfx
{
	class Mesh;

	class Node
	{
	private:
		std::string name;
		uint32_t id = 0;
		std::vector<Mesh> meshes;
		std::vector<std::unique_ptr<Node>> children;
		glm::mat4 transform = {};
		glm::mat4 parentTransform = {};
	};
}
