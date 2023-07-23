#pragma once
#include "Mesh.h"

namespace tryn::gfx
{
	class StaticMesh : public Mesh
	{
	public:
		// Make a mesh via a loading a file
		StaticMesh(std::string path , glm::vec3 scale = {1.0f,1.0f,1.0f});
		// Make a mesh by passing a vertex buffer and index buffer
		StaticMesh(VertexBuffer&& buffer, std::vector<int>&& indices, std::string tag);
		StaticMesh(std::vector<VertexBuffer>&& buffers, std::vector<int>&& indices, std::string tag);
		bool IsStatic() const override;
		void MakeBindables(gfx::IGraphics& gfx) override;
	};
}