#pragma once
#include "Mesh.h"
#include <Core/src/gfx/Shape.h>
struct aiMesh;

namespace tryn::gfx
{
	template <typename T>
	concept Shape = std::derived_from<T, Shape3D>;

	class StaticMesh : public Mesh
	{
	public:
		// Make a mesh via assimp meshes and materials
		StaticMesh(const IGraphics& gfx, const aiMesh& mesh, std::string_view tag, const Material* pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);

		// Make a mesh via Shape
		StaticMesh(const IGraphics& gfx, const Shape3D& shape, std::string_view tag, const Material* pMaterial = nullptr, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);

		[[nodiscard]] MeshType Type() const override;
	};
}