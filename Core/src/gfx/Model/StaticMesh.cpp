#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <format>
#include <utility>

namespace tryn::gfx
{
	StaticMesh::StaticMesh(IGraphics& gfx, const Material& material, const aiMesh& mesh, std::string_view tag, glm::vec3 scale)
	{
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		const auto vertexBuffer = material.ExtractVertices(mesh);
		const auto indices = material.ExtractIndices(mesh);

		indexCount = static_cast<uint32_t>(indices.size());

		pVertexBuffer = IVertexBuffer::Resolve(gfx, std::make_shared<VertexBuffer>(vertexBuffer), this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<std::vector<int>>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);
	}

	bool StaticMesh::IsStatic() const
	{
		return true;
	}
}