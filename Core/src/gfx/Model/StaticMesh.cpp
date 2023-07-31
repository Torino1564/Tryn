#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Assimp.h>
#include <format>
#include <utility>

namespace tryn::gfx
{
	StaticMesh::StaticMesh(IGraphics& gfx, aiMesh* mesh, std::string_view tag, glm::vec3 scale)
	{
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		gfx::VertexLayout vertexLayout;


		if (mesh->HasPositions()) vertexLayout.AppendElement(VertexLayout::VertexElement::Position3D);
		if (mesh->HasNormals()) vertexLayout.AppendElement(VertexLayout::VertexElement::Normal);
		if (mesh->HasTangentsAndBitangents())
		{
			vertexLayout.AppendElement(VertexLayout::VertexElement::Tangent);
			vertexLayout.AppendElement(VertexLayout::VertexElement::Bitangent);
		}

		gfx::VertexBuffer vertexBuffer(std::move(vertexLayout), mesh->mNumVertices);

		if (mesh->HasPositions())
		{
			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				const auto pos = mesh->mVertices[i];
				glm::vec3 position(pos.x * scale.x, pos.y * scale.y, pos.z * scale.z);
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Position3D>() = position;
			}
		}
		if (mesh->HasNormals())
		{
			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				const auto norm = mesh->mNormals[i];
				glm::vec3 normal(norm.x, norm.y, norm.z);
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Normal>() = normal;
			}
		}
		if (mesh->HasTangentsAndBitangents())
		{
			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				const auto tan = mesh->mTangents[i];
				const auto btan = mesh->mBitangents[i];
				glm::vec3 tangent(tan.x, tan.y, tan.z);
				glm::vec3 bitangent(btan.x, btan.y, btan.z);
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Tangent>() = tangent;
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Bitangent>() = bitangent;
			}
		}

		std::vector<int> indices;
		indices.resize(mesh->mNumFaces * static_cast<size_t>(3));

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			const auto& triangle = mesh->mFaces[i];
			indices[3 * i] = triangle.mIndices[0];
			indices[(3 * i) + 1] = triangle.mIndices[1];
			indices[(3 * i) + 2] = triangle.mIndices[2];
		}
		indexCount = static_cast<uint32_t>(indices.size());

		BufferArray bfArray;
		bfArray.push_back(std::pair<std::string, std::shared_ptr<VertexBuffer>>{"?", std::make_shared<VertexBuffer>(vertexBuffer)});

		pVertexBuffer = IPolyVBuffer::Resolve(gfx, bfArray , this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, pCpuIndexData);
		pTopology = IPrimitiveTopology::Resolve(gfx);
	}

	bool StaticMesh::IsStatic() const
	{
		return true;
	}
}

