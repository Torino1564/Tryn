#pragma once
#include "Mesh.h"
#include <Core/src/gfx/Shape.h>

#include "Core/src/gfx/Vertex.h"

namespace Microsoft::glTF
{
	struct MeshPrimitive;
}

struct aiMesh;

namespace tryn::gfx
{
	class WinGLTFLoaderContext;
	template <typename T>
	concept Shape = std::derived_from<T, Shape3D>;

	class StaticMesh : public Mesh
	{
	public:
		// Make a mesh via assimp meshes and materials
		StaticMesh(const IGraphics& gfx, const aiMesh& mesh, std::string_view tag, const std::shared_ptr<Material>& pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);

		// Make a mesh via Shape
		StaticMesh(const IGraphics& gfx, const Shape3D& shape, std::string_view tag, const std::shared_ptr<Material>& pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);

		// Make a mesh via gltf primitives
		StaticMesh(const IGraphics& gfx, const Microsoft::glTF::MeshPrimitive& mesh, const gfx::WinGLTFLoaderContext& context, std::string_view tag, const std::shared_ptr<Material>& pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);

		[[nodiscard]] MeshType Type() const override;

		void SubmitBoned(const IGraphics& gfx, const glm::mat4& finalTransform, std::span<const glm::mat4> boneTransforms) override;

		void EnableOrAddTechnique(const IGraphics& gfx, utl::UUID_t techniqueUUID, std::span<uint16_t> materialIndex) override;
		void AddTechnique(const IGraphics& gfx, utl::UUID_t techniqueUUID, std::span<uint16_t> materialIndex, bool enabled = true) override;
	};
}