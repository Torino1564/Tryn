#include "CPUBuffer.h"

#include <assimp/mesh.h>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

namespace tryn::gfx
{
	void CPUBuffer::Resize(const std::size_t newSize)
	{
		throw CPUBufferException(L"This CPU Buffer cannot be resized!");
	}

	FlatBuffer::FlatBuffer(int byteSize)
	{
		buffer.resize(byteSize, (std::byte)0);
	}

	std::size_t FlatBuffer::Stride() const noexcept
	{
		return 0;
	}

	std::size_t FlatBuffer::Size() const noexcept
	{
		return buffer.size();
	}

	void FlatBuffer::Resize(const std::size_t newSize)
	{
		buffer.resize(newSize);
	}

	IndexBuffer::IndexBuffer(int indexCount)
	{
		buffer.resize(indexCount);
		dirty = false;
	}

	IndexBuffer::IndexBuffer(const std::vector<uint32_t>& data)
	{
		buffer = data;
		dirty = false;
	}

	IndexBuffer::IndexBuffer(const aiMesh& mesh)
	{
		std::vector<uint32_t> indices;
		indices.resize(3 * mesh.mNumFaces);

		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& triangle = mesh.mFaces[i];
			indices[3 * i] = triangle.mIndices[0];
			indices[(3 * i) + 1] = triangle.mIndices[1];
			indices[(3 * i) + 2] = triangle.mIndices[2];
		}

		buffer = indices;
		dirty = false;
	}

	IndexBuffer::IndexBuffer(const fastgltf::Primitive& primitive, fastgltf::Asset& asset)
	{
		auto& accessorIndex = primitive.indicesAccessor;
		if (!accessorIndex.has_value())
		{
			throw CPUBufferException(L"Primitive has no indices accessor!");
		}
		auto& accessor = asset.accessors[*accessorIndex];
		if (!accessor.bufferViewIndex)
		{
			throw CPUBufferException(L"Accessor has no buffer view index!");
		}
		unsigned int i = 0;
		Resize(accessor.count);
		for (const auto& element : fastgltf::iterateAccessor<std::uint32_t>(asset, accessor))
		{
			buffer[i++] = element;
		}

		dirty = false;
	}

	std::size_t IndexBuffer::Stride() const noexcept
	{
		return stride;
	}

	std::size_t IndexBuffer::Size() const noexcept
	{
		return buffer.size();
	}

	void IndexBuffer::Resize(const std::size_t newSize)
	{
		buffer.resize(newSize);
	}
}

