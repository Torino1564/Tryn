#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <format>
#include <utility>

namespace tryn::gfx
{
	StaticMesh::StaticMesh(IGraphics& gfx, std::shared_ptr<Material> pMaterial, const aiMesh& mesh, std::string_view tag, glm::vec3 scale, std::optional<std::uint16_t> meshID)
	{
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		ID = meshID.value_or(0);
		auto vertexBuffer = pMaterial->ExtractVertices(mesh);
		vertexBuffer.SetClean();
		const auto indices = pMaterial->ExtractIndices(mesh);

		indexCount = static_cast<uint32_t>(indices.Size());

		pVertexBuffer = IVertexBuffer::Resolve(gfx, std::make_shared<VertexBuffer>(vertexBuffer), this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);
		SetMaterial(pMaterial);
	}

	MeshType StaticMesh::Type() const
	{
		return MeshType::Static;
	}
}