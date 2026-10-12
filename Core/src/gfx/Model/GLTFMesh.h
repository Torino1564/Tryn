#pragma once
#include "Mesh.h"

// This class is an adaptor for the concept of GLTF primitives.

namespace Microsoft::glTF
{
	struct Mesh;
}

namespace fastgltf
{
	class Mesh;
	class Asset;
}

namespace tryn::gfx
{
	class WinGLTFLoaderContext;

	class GLTFMesh : public Mesh
	{
	public:
		GLTFMesh(const IGraphics& gfx, const Microsoft::glTF::Mesh& mesh, const gfx::WinGLTFLoaderContext& context, std::string_view tag, std::shared_ptr<Material> pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);
		GLTFMesh(const IGraphics& gfx, const fastgltf::Mesh& mesh, fastgltf::Asset& asset, std::string_view tag, std::shared_ptr<Material> pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);
		void Submit(const IGraphics& gfx, const glm::mat4& transform) override;
		void Submit(const IGraphics& gfx, std::span<const glm::mat4> transforms, InstancedModelParent& parent) override;
		void SubmitBoned(const IGraphics& gfx, const glm::mat4& finalTransform, std::span<const glm::mat4> boneTransforms) override;
		[[nodiscard]] MeshType Type() const override;

		void AddTechnique(const IGraphics& gfx, utl::UUID_t techniqueUUID, std::span<uint16_t> materialIndex, bool enabled) override;
		void EnableOrAddTechnique(const IGraphics& gfx, utl::UUID_t techniqueUUID, std::span<uint16_t> materialIndex) override;

	};
}
