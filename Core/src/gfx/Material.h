#pragma once
#include <core/src/gfx/IGraphics.h>
#include <core/src/gfx/RenderQueue/Technique.h>
#include <Core/src/gfx/Vertex.h>
#include <filesystem>
#include <format>
#include <Core/src/gfx/IBufferFwd.h>

struct aiMaterial;
struct aiMesh;

namespace tryn::gfx
{
	class IIndexBuffer;

	class Material
	{
	public:
		Material(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, Techniques defaultTechnique = Techniques::Phong);
		VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
		std::vector<int> ExtractIndices(const aiMesh& mesh) const noexcept;
		std::vector<Technique> GetTechniques() const noexcept;

	private:
		VertexLayout vLayout;
		std::vector<Technique> techniques;
		std::string name;
	};
}
