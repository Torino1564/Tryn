#include <Core/src/gfx/Bindables/VertexBuffer.h>

namespace tryn::gfx {

	std::shared_ptr<IVertexBuffer> IVertexBuffer::Resolve(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag)
	{
		return BindablePool::Resolve<IVertexBuffer>(gfx, cpuBuffer, tag);
	}


	std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag)
	{
		if (tag == "?") return tag;
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::stringstream ss;
		ss << typeStr << "#VertexBuffer#" << std::to_string(cpuBuffer->ByteSize()) << "#";

		for (auto& element : cpuBuffer->GetLayout().Elements)
		{
			ss << element.GetName() << element.Id() << element.Index();
		};
		ss << "#" << tag;

		return ss.str();
	}
	
	const VertexBuffer& IVertexBuffer::GetVertexBuffer() const
	{
		return static_cast<const VertexBuffer&>(*pCPUBuffer);
	}

	VertexBuffer& IVertexBuffer::GetVertexBuffer() const
	{
		pCPUBuffer->SetDirty();
		return static_cast<VertexBuffer&>(*pCPUBuffer);
	}

	const VertexLayout& IVertexBuffer::GetLayout() const
	{
		return GetVertexBuffer().GetLayout();
	}
}