#pragma once
#include <core/src/gfx/IGraphics.h>
#include <core/src/gfx/RenderQueue/Technique.h>
#include <Core/src/gfx/Vertex.h>
#include <filesystem>

struct aiMaterial;
struct aiMesh;

namespace tryn::gfx
{
	class IIndexBuffer;
	class IVertexBuffer;

	class Material
	{
	public:
		Material(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path);
		VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
		std::vector<int> ExtractIndices(const aiMesh& mesh) const noexcept;
		std::shared_ptr<IVertexBuffer> MakeVertexBindable(IGraphics& gfx, const aiMesh& mesh, glm::vec3 scale = {1.0f,1.0f,1.0f}) const;
		std::shared_ptr<IIndexBuffer> MakeIndexBindable(IGraphics& gfx, const aiMesh& mesh) const;
		std::vector<Technique> GetTechniques() const noexcept;

	private:
		VertexLayout vLayout;
		std::vector<Technique> techniques;
		std::string name;
		std::string modelPath;
	};
}
