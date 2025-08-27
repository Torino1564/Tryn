#pragma once
#include <memory>
#include <string>
#include <vector>
#include <core/third/glm/glm.hpp>
#include <span>
#include "Core/src/utl/StringHasher.h"

namespace tryn::gfx
{
	class Mesh;
	class IGraphics;
	class InstancedModelParent;
	class Model;

	class Node
	{
	public:
		Node(int id, std::string_view name, std::vector<uint16_t> meshIds, glm::mat4 transform, Model* pModel, bool isSkeleton = false);
		void Submit(const IGraphics& gfx, const glm::mat4& accumulatedTransform) const;
		void Submit(const IGraphics& gfx, std::span<const glm::mat4> accumulatedTransforms, InstancedModelParent& parent) const;
		void SubmitBoned(const IGraphics& gfx, const glm::mat4& accumulatedTransform, std::span<const glm::mat4> boneTransforms) const;
		void AddChildId(uint32_t index);
		void SetMeshSpan(std::span<std::shared_ptr<Mesh>> meshSpan);
		std::vector<uint32_t>& GetChildrenIds();
	private:
		bool isSkeleton = false;
		std::string name;
		uint32_t id = 0;
		std::vector<std::uint16_t> meshIds;
		std::span<std::shared_ptr<Mesh>> meshes;
		std::vector<uint32_t> childrenIds;
		Model* pModel = nullptr;
		glm::mat4 transform = {};
		glm::mat4 appliedTransform = {};
	};
}
