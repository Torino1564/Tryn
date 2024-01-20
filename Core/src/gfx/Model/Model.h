#pragma once
#include <memory>
#include "Node.h"
#include <Core/src/gfx/IGraphics.h>
#include "Mesh.h"
#include <Core/src/gfx/ImguiManager.h>
#include <Core/src/gfx/Animation/Bone.h>
#include <Core/src/gfx/Animation/BonedMesh.h>

namespace tryn::gfx
{
	struct Settings
	{
		glm::vec3 angles; // (yaw, pitch, roll)
		glm::vec3 position; // (x, y, z)
	};

	class Model
	{
		friend class InstancedModelParent;
	public:
		Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, Techniques defaultTechnique = Techniques::Phong, bool instanced = false );
		void Submit(const glm::mat4& entityTransform);
		void Submit(const glm::mat4& entityTransform, std::span<const glm::mat4> boneTransforms);
		void SpawnControlWindow();
		void AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string&);
		glm::vec3 GetPosition() const;
		std::uint16_t GetMeshAmount() const;
		ani::BonedMesh* GetMainMesh();
	private:
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root = false);
		void ParseSkeleton(const aiNode& boneRoot);
		void ParseBone(const aiNode& bone, const uint32_t parentID);
		Settings settings = {};
	private:
		std::optional<ani::Skeleton> skeleton = std::nullopt;
		std::uint16_t meshCounter = 0;
		gfx::IGraphics& gfx;
		std::string name;
		std::unique_ptr<Node> root;
		std::vector<std::shared_ptr<Mesh>> pMeshes;
	};
}
