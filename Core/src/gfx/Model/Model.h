#pragma once
#include <memory>
#include "Node.h"
#include "Mesh.h"
#include <Core/src/gfx/Animation/Bone.h>
#include <Core/src/gfx/Animation/BonedMesh.h>
#include <Core/src/gfx/Model/StaticMesh.h>
#include <concepts>
#include <Core/src/gfx/Assimp.h>
#include "ModelException.h"
#include <Core/src/gfx/Animation/AnimationManager.h>
#include "Core/src/gfx/Render/Techniques/ForwardPhong.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/ser/StreamIO.h>

namespace tryn::gfx
{
	class IGraphics;
	template <typename T>
	concept DerivedFromTechniqueBase = std::derived_from<typename T::Type, TechniqueBase>;

	struct Settings
	{
		glm::vec3 angles; // (yaw, pitch, roll)
		glm::vec3 position; // (x, y, z)
	};

	class Model
	{
		friend class InstancedModelParent;
	public:
		Model(const gfx::IGraphics& gfx, std::string_view path, std::span<const utl::UUID_t> techniqueUUIDs = {}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f}, const bool instanced = false);
		template <typename... Techniques>
			requires std::derived_from<Techniques..., TechniqueBase>
		static std::unique_ptr<Model> Make(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false);
		~Model();
		Model(Model&&) = default;
		void Submit(const glm::mat4& entityTransform);
		void Submit(const glm::mat4& entityTransform, std::span<const glm::mat4> boneTransforms) const;
		void SpawnControlWindow();
		void AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string&) const;
		glm::vec3 GetPosition() const;
		std::uint16_t GetMeshAmount() const;
		ani::BonedMesh* GetMainMesh() const;
		const gfx::IGraphics* GetGfx() const;
		ani::Skeleton& GetSkeleton();
	private:
		void TinyGltfInitialization(const gfx::IGraphics& gfx, const std::filesystem::path& path, std::span<const utl::UUID_t> techniqueUUIDs = {}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f}, const bool instanced = false);
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root = false);
		void ParseSkeleton(const aiNode& boneRoot);
		void ParseBone(const aiNode& bone, const uint32_t parentID);
		Settings settings = {};
	private:
		std::optional<ani::Skeleton> skeleton = std::nullopt;
		std::uint16_t meshCounter = 0;
		const gfx::IGraphics& gfx;
		std::string name = {};
		std::unique_ptr<Node> root = {};
		std::vector<std::shared_ptr<Mesh>> pMeshes = {};
		friend class Serializer;

	public:
		struct Serializer : public tryn::ser::Serializer<Model>
		{
			static void Write(const tryn::ser::StreamWriter& streamWriter, const Model& data, const bool binary = true,
			                  const std::string& name = "");

			static Model Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, ser::ExtraDataPack* pExtraData = nullptr);

			static void Read(Model& data, const tryn::ser::StreamReader& streamReader, const bool binary = true, ser::ExtraDataPack* pExtraData = nullptr);
		};
	};

	template <typename... Techniques>
	requires std::derived_from<Techniques..., TechniqueBase>
	std::unique_ptr<Model> Model::Make(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, bool instanced)
	{
		static constexpr auto arr =  {ZT_TYPE_UUID(Techniques)...};
		return std::make_unique<Model>(gfx, path, arr, scale, instanced);
	}
}
