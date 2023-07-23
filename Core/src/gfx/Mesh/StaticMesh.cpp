#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/Assimp.h>
#include <format>

namespace tryn::gfx
{
	StaticMesh::StaticMesh(std::string path, glm::vec3 scale = { 1.0f,1.0f,1.0f })
	{
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", path, scale.x, scale.y, scale.z);
		}
		else
		{
			tag = path;
		}

		const auto pModel = gfx::AssimpManager::Get().ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);

		if (pModel->mNumMeshes != 1) trylog.warn(L"The static mesh was created but there were some unused meshes!");

		// TODO Deal with no existing path error

		gfx::VertexLayout vertexLayout;

		const auto aiMesh = pModel->mMeshes[1];

		if (aiMesh->HasPositions()) vertexLayout.AppendElement(VertexLayout::VertexElement::Position3D);
		if (aiMesh->HasNormals()) vertexLayout.AppendElement(VertexLayout::VertexElement::Normal);
		if (aiMesh->HasTangentsAndBitangents())
		{
			vertexLayout.AppendElement(VertexLayout::VertexElement::Tangent);
			vertexLayout.AppendElement(VertexLayout::VertexElement::Bitangent);
		}

		gfx::VertexBuffer vertexBuffer(std::move(vertexLayout), aiMesh->mNumVertices);

		if (aiMesh->HasPositions())
		{
			for (int i = 0; i < aiMesh->mNumVertices; i++)
			{
				const auto pos = aiMesh->mVertices[i];
				glm::vec3 position(pos.x * scale.x, pos.y * scale.y , pos.z * scale.z);
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Position3D>() = position;
			}
		}
		if (aiMesh->HasNormals())
		{
			for (int i = 0; i < aiMesh->mNumVertices; i++)
			{
				const auto norm = aiMesh->mNormals[i];
				glm::vec3 normal(norm.x, norm.y, norm.z);
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Normal>() = normal;
			}
		}
		if (aiMesh->HasTangentsAndBitangents())
		{
			for (int i = 0; i < aiMesh->mNumVertices; i++)
			{
				const auto tan = aiMesh->mTangents[i];
				const auto btan = aiMesh->mBitangents[i];
				glm::vec3 tangent(tan.x, tan.y, tan.z);
				glm::vec3 bitangent(btan.x, btan.y, btan.z);
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Tangent>() = tangent;
				vertexBuffer[i].Attr<VertexLayout::VertexElement::Bitangent>() = bitangent;
			}
		}

		std::vector<int> indices;
		indices.resize(aiMesh->mNumFaces * (size_t)3);

		for (size_t i = 0; i < aiMesh->mNumFaces; i++)
		{
			const auto& triangle = aiMesh->mFaces[i];
			indices[3 * i] = triangle.mIndices[0];
			indices[(3 * i) + 1] = triangle.mIndices[1];
			indices[(3 * i) + 2] = triangle.mIndices[2];
		}
		indexCount = indices.size();

		std::vector<VertexBuffer> temp;
		temp.emplace_back(std::move(vertexBuffer));
		pCpuVertexData = std::make_shared<std::vector<VertexBuffer>>(temp);
		pCpuIndexData = std::make_shared<std::vector<int>>(indices);
	}
	StaticMesh::StaticMesh(VertexBuffer&& buffer, std::vector<int>&& indices, std::string tag)
	{
		this->tag = tag;
		std::vector<VertexBuffer> temp;
		temp.emplace_back(buffer);
		pCpuVertexData = std::make_shared<std::vector<VertexBuffer>>(std::move(temp));
		pCpuIndexData = std::make_shared<std::vector<int>>(indices);
		indexCount = static_cast<int>(pCpuIndexData->size());
	}
	StaticMesh::StaticMesh(std::vector<VertexBuffer>&& buffers, std::vector<int>&& indices, std::string tag)
	{
		this->tag = tag;
		pCpuVertexData = std::make_shared<std::vector<VertexBuffer>>(buffers);
		pCpuIndexData = std::make_shared<std::vector<int>>(indices);
		indexCount = static_cast<int>(pCpuIndexData->size());
	}
	bool StaticMesh::IsStatic() const
	{
		return true;
	}
	void StaticMesh::MakeBindables(gfx::IGraphics& gfx)
	{
		BufferArray bfarray;
		for (auto& buffer : *pCpuVertexData)
		{
			std::pair<std::string, std::shared_ptr<gfx::VertexBuffer>> pair("?", std::make_shared<VertexBuffer>(buffer));
			bfarray.push_back(pair);
		}

		pVertexBuffer = IPolyVBuffer::Resolve(gfx, bfarray);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, pCpuIndexData);

		hasBinds = true;
	}
}

