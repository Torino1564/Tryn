#pragma once
#include "Mesh.h"

struct aiMesh;


namespace tryn::gfx
{
	class StaticMesh final : public Mesh
	{
	public:
		// Make a mesh via assimp meshes and materials
		StaticMesh(IGraphics& gfx, aiMesh* mesh, std::string_view tag  = "?", glm::vec3 scale = glm::vec3{1.0f,1.0f,1.0f});
		[[nodiscard]] bool IsStatic() const override;
	};
}