#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/CPUBuffer.h>

namespace tryn::gfx
{
	std::shared_ptr<IIndexBuffer> IIndexBuffer::Resolve(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, const std::string& tag)
	{
		return BindablePool::Resolve<IIndexBuffer>(gfx, indices, tag);
	}
	std::string IIndexBuffer::GenerateID(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, const std::string& tag)
	{
		if (tag == "?") return tag;
		decltype(auto) typeStr = gfx.GetAPIString();
		std::string UID(typeStr);
		UID += "#IndexBuffer#";
		UID += std::to_string(indices->Size());
		UID += '#';
		UID += tag;
		return UID;
	}
	const IndexBuffer& IIndexBuffer::GetIndexBuffer() const
	{
		return static_cast<const IndexBuffer&>(*pCPUBuffer);
	}
	IndexBuffer& IIndexBuffer::GetIndexBuffer()
	{
		pCPUBuffer->SetDirty();
		return static_cast<IndexBuffer&>(*pCPUBuffer);
	}

	std::string_view IIndexBuffer::GetPath() const
	{
		return path;
	}

	std::string_view IIndexBuffer::GetTag() const
	{
		return tag;
	}
}