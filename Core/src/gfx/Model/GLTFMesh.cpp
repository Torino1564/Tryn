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
		for (auto& primitive : children)
		{
			primitive->Submit(gfx, transform);
		}
	}

	void GLTFMesh::Submit(const IGraphics& gfx, const std::span<const glm::mat4> transforms, InstancedModelParent& parent)
	{
		for (auto& primitive : children)
		{
			primitive->Submit(gfx, transforms, parent);
		}
	}

	void GLTFMesh::SubmitBoned(const IGraphics& gfx, const glm::mat4& finalTransform,
		std::span<const glm::mat4> boneTransforms)
	{
		trylog.warn(L"Cannot submit a gltf mesh with bone transforms! (Unimplemented)");
		Submit(gfx, finalTransform);
	}

	GLTFMesh::GLTFMesh(const IGraphics& gfx, const Microsoft::glTF::Mesh& mesh,
	                   const gfx::WinGLTFLoaderContext& context, std::string_view tag, std::shared_ptr<Material> pMaterial,
	                   glm::vec3 scale, std::optional<std::uint16_t> meshID)
	{
		ID = meshID.value_or(0);
		isParentMesh = true;
		pMaterials.emplace_back(pMaterial);
		for (const auto& primitive : mesh.primitives)
		{
			children.push_back(std::move(std::make_shared<StaticMesh>(gfx, primitive, context, tag, pMaterial, scale)));
		}
	}

	void GLTFMesh::EnableOrAddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const std::span<uint16_t> materialIndex)
	{
		for (const auto& mesh : children)
		{
			mesh->EnableOrAddTechniqueEx(gfx, techniqueUUID, false, materialIndex);
		}
	}

	void GLTFMesh::AddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const std::span<uint16_t> materialIndex, const bool enabled)
	{
		for (const auto& mesh : children)
		{
			mesh->AddTechniqueEx(gfx, techniqueUUID, false, materialIndex, enabled);
		}
	}
}
