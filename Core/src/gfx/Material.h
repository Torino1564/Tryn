#pragma once
#include <core/src/gfx/IGraphics.h>
#include <core/src/gfx/Render/Technique.h>
#include <Core/src/gfx/Vertex.h>
#include <filesystem>
#include <format>
#include <Core/src/gfx/IBufferFwd.h>

struct aiMaterial;
struct aiMesh;

namespace tryn::gfx
{
	class Material
	{
	public:
		Material(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, Techniques defaultTechnique = Techniques::Phong, bool instanced = false, bool skinned = false);
		VertexBuffer ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton = nullptr) const noexcept;
		IndexBuffer ExtractIndices(const aiMesh& mesh) const noexcept;
		std::vector<Technique> GetTechniques() const noexcept;

	private:
		VertexLayout vLayout;
		std::vector<Technique> techniques;
		std::string name;
	};
}
