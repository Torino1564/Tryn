#pragma once
#include "Mesh.h"
#include <Core/src/gfx/Material.h>

struct aiMesh;

namespace tryn::gfx
{
	class StaticMesh final : public Mesh
	{
	public:
		// Make a mesh via assimp meshes and materials
		StaticMesh(IGraphics& gfx, const Material& material, const aiMesh& mesh, std::string_view tag, glm::vec3 scale = glm::vec3{ 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);
		[[nodiscard]] MeshType Type() const override;
	};
}