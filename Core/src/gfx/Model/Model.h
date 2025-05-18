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

#include "Core/src/gfx/win/gltfSDK.h"

namespace Microsoft::glTF
{
	struct Node;
	struct Scene;
	class Document;
}

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
		friend class Serializer;
		friend class Node;

	public:

		// Avoid using the default constructor. Use the parametrized constructor or factory function instead
		Model();
		Model(const gfx::IGraphics& gfx, std::string_view path, std::span<const utl::UUID_t> techniqueUUIDs = {std::array{ZT_TYPE_UUID(ForwardPhong)}}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f}, const bool instanced = false);
		template <typename... Techniques>
			requires std::derived_from<Techniques..., TechniqueBase>
		static std::unique_ptr<Model> Make(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false);
		~Model();
		Model(Model&&) = default;
		// Submit the model to the render pipeline
		void Submit(const glm::mat4& entityTransform);
		void Submit(const glm::mat4& entityTransform, std::span<const glm::mat4> boneTransforms) const;

		// Adds a bindable with an identifier that will be offered to the existing techinques to see if they link with it.
		void AddPerTechniqueBindable(const std::shared_ptr<IBindable>& pBindable, const std::string& identifier);
		// Adds and enables the specified techniques on all meshes
		void AddOrEnableTechniques(std::span<const utl::UUID_t> techniqueUUIDs);
		void SpawnControlWindow();
		void AddAnimation(const std::shared_ptr<ani::Animation>& pAnimation, const std::string&) const;
		glm::vec3 GetPosition() const;
		std::uint16_t GetMeshAmount() const;
		ani::BonedMesh* GetMainMesh() const;
		const gfx::IGraphics* GetGfx() const;
		ani::Skeleton& GetSkeleton();
	private:
		void GltfInitialization(const gfx::IGraphics& gfx, const std::filesystem::path& path, std::span<const utl::UUID_t> techniqueUUIDs = {}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f}, const bool instanced = false);
		std::uint32_t ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root = false);
		std::uint32_t ParseNode(int& nextId, const Microsoft::glTF::Node& node, const WinGLTFLoaderContext& context, glm::vec3 scale, bool root = false);
		void ParseSkeleton(const aiNode& boneRoot);
		void ParseBone(const aiNode& bone, const uint32_t parentID);
		Settings settings = {};

	private:

		// Skeleton optional
		std::optional<ani::Skeleton> skeleton = std::nullopt;
		std::uint16_t meshCounter = 0;
		const IGraphics* pGfx = nullptr;
		std::string name = {};

		// Nodes
		std::vector<Node> nodes = {};

		// Index of root node
		std::uint32_t rootId;

		// Meshes utilized by Nodes
		std::vector<std::shared_ptr<Mesh>> pMeshes = {};

		// Materials utilized by child meshes
		std::vector<std::shared_ptr<Material>> pMaterials;

		// Techniques utilized by child meshes
		std::vector<utl::UUID_t> techniques;

		// Added bindables. The steps inside the techniques can link to these and bind them when rendered.
		std::vector<std::pair<std::string, std::shared_ptr<IBindable>>> pAddedBindables;
	};

	template <typename... Techniques>
	requires std::derived_from<Techniques..., TechniqueBase>
	std::unique_ptr<Model> Model::Make(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, bool instanced)
	{
		static constexpr auto arr =  {ZT_TYPE_UUID(Techniques)...};
		return std::make_unique<Model>(gfx, path, arr, scale, instanced);
	}
}

namespace tryn::ser
{
	void Serialize(StreamIO& io, gfx::Model* pModel, bool binary, const std::string& name);
}
