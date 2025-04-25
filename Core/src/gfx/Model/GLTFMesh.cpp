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
		for (const auto& primitive : mesh.primitives)
		{
			pPrimitives.push_back(std::make_shared<StaticMesh>(gfx, primitive, context, tag, pMaterial, scale));
		}
	}
}
