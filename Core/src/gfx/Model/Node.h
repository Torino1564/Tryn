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
		Node(int id, std::string_view name, std::vector<uint16_t> meshIds, glm::mat4 transform, bool isSkeleton = false);
		void Submit(IGraphics& gfx, glm::mat4 accumulatedTransform);
		void Submit(IGraphics& gfx, std::span<const glm::mat4> accumulatedTransforms, InstancedModelParent& parent);
		void AddChild(Node);
		void SetMeshSpan(std::span<std::shared_ptr<Mesh>> meshSpan);
		std::vector<Node>& GetChildren();
	private:
		bool isSkeleton = false;
		std::string name;
		uint32_t id = 0;
		std::vector<std::uint16_t> meshIds;
		std::span<std::shared_ptr<Mesh>> meshes;
		std::vector<Node> children;
		glm::mat4 transform = {};
		glm::mat4 appliedTransform = {};
	};
}
