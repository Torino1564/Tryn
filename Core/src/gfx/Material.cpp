#include "TrynPCH.h"
#include "Material.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <Core/src/gfx/Render/Techniques/ForwardPhong.h>

namespace tryn::gfx
{
	Material Material::MakeDefault(const IGraphics& gfx)
	{
		static const aiMaterial emptyMat = {};
		static const std::filesystem::path emptyPath = {};
		return Make<ForwardPhong>(gfx, emptyMat, emptyPath);
	}

	Material::Material(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path,
		std::span<const utl::UUID_t> techniqueUUIDs, const aiScene* pScene, const bool instanced, const bool skinned)
			: pScene(pScene)
	{
		const auto rootPath = path.parent_path().string() + "\\";

		if (techniqueUUIDs.size() == 0)
		{
			AddTechnique(ZT_TYPE_UUID(ForwardPhong), gfx, material, rootPath, instanced, skinned);
		}

		for (auto techniqueUUID : techniqueUUIDs)
		{
			AddTechnique(techniqueUUID, gfx, material, rootPath, instanced, skinned);
		}

	}

	VertexBuffer Material::ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton) const noexcept
	{
		return { vLayout, mesh, skeleton};
	}
	IndexBuffer Material::ExtractIndices(const aiMesh& mesh) noexcept
	{
		std::vector<uint32_t> indices;
		indices.resize(mesh.mNumFaces * 3);

		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& triangle = mesh.mFaces[i];
			indices[3 * i] = triangle.mIndices[0];
			indices[(3 * i) + 1] = triangle.mIndices[1];
			indices[(3 * i) + 2] = triangle.mIndices[2];
		}
		return { indices };
	}

	VertexBuffer Material::ExtractVertices(const Shape3D& mesh) const noexcept
	{
		return { vLayout, mesh};
	}

	IndexBuffer Material::ExtractIndices(const Shape3D& mesh) const noexcept
	{
		return IndexBuffer(mesh.Indices());
	}

	std::vector<std::shared_ptr<TechniqueBase>> Material::GetTechniques() const noexcept
	{
		return pTechniques;
	}

	void Material::AddTechnique(utl::UUID_t techniqueUUID, const IGraphics& gfx, const aiMaterial& material,
		const std::string& path, bool instanced, bool skinned)
	{
		pTechniques.push_back(TechniquePool::ConstructTechnique(techniqueUUID, *this, material, gfx, path,  instanced, skinned));
	}

	Material::Material(const aiScene* pScene)
		: pScene(pScene)
	{
	}
}
