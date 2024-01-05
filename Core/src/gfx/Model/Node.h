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
	public:
		Node(int id, std::string_view name, std::vector<Mesh*> pMeshes, glm::mat4 transform);
		void Submit(IGraphics& gfx, glm::mat4 accumulatedTransform);
		void Submit(IGraphics& gfx, std::span<const glm::mat4> accumulatedTransforms, InstancedModelParent& parent);
		void AddChild(Node);
	private:
		std::string name;
		uint32_t id = 0;
		std::vector<Mesh*> pMeshes;
		std::vector<Node> children;
		glm::mat4 transform = {};
		glm::mat4 appliedTransform = {};
	};
}
