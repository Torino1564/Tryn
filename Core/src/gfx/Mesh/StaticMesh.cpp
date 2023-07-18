#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PolyVBuffer.h>

namespace tryn::gfx
{
	StaticMesh::StaticMesh(std::string path)
	{
		// TODO ASSIMP
		tag = path;
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

