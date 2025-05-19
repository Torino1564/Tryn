#include "TrynPCH.h"
#include "GLTFMesh.h"

#include <GLTFSDK/GLTF.h>

#include "StaticMesh.h"
#include "Core/src/gfx/Material.h"

namespace tryn::gfx
{
	MeshType GLTFMesh::Type() const
	{
		return MeshType::GLTF;
	}

	void GLTFMesh::Submit(const IGraphics& gfx, const glm::mat4& transform)
	{
		for (auto& primitive : pPrimitives)
		{
			primitive->Submit(gfx, transform);
		}
	}

	void GLTFMesh::Submit(const IGraphics& gfx, const std::span<const glm::mat4> transforms, InstancedModelParent& parent)
	{
		for (auto& primitive : pPrimitives)
		{
			primitive->Submit(gfx, transforms, parent);
		}
	}

	GLTFMesh::GLTFMesh(const IGraphics& gfx, const Microsoft::glTF::Mesh& mesh,
	                   const gfx::WinGLTFLoaderContext& context, std::string_view tag, std::shared_ptr<Material> pMaterial,
	                   glm::vec3 scale, std::optional<std::uint16_t> meshID)
	{
		ID = meshID.value_or(0);
		pMaterials.emplace_back(pMaterial);
		for (const auto& primitive : mesh.primitives)
		{
			pPrimitives.push_back(std::move(std::make_shared<StaticMesh>(gfx, primitive, context, tag, pMaterial, scale)));
		}
	}

	void GLTFMesh::EnableOrAddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const std::span<uint16_t> materialIndex)
	{
		for (const auto& mesh : pPrimitives)
		{
			mesh->EnableOrAddTechniqueEx(gfx, techniqueUUID, false, materialIndex);
		}
	}

	void GLTFMesh::AddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const std::span<uint16_t> materialIndex, const bool enabled)
	{
		for (const auto& mesh : pPrimitives)
		{
			mesh->AddTechniqueEx(gfx, techniqueUUID, false, materialIndex, enabled);
		}
	}
}
