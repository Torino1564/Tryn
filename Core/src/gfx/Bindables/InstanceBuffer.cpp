#include <Core/src/gfx/Bindables/InstanceBuffer.h>
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/CPUBuffer.h>

namespace tryn::gfx
{
	std::shared_ptr<IInstanceBuffer> IInstanceBuffer::Resolve(const IGraphics& gfx, const ConstantBufferLayout::Node& node, int slot, const std::size_t num_instances)
	{
		return BindablePool::Resolve<IInstanceBuffer>(gfx, node, slot, num_instances);
	}

	std::string IInstanceBuffer::GenerateID(const IGraphics& gfx, const ConstantBufferLayout::Node& node, int slot, const std::size_t num_instances)
	{
		return "?";
	}

	const ConstantBuffer& IInstanceBuffer::GetConstantBuffer() const
	{
		return static_cast<const ConstantBuffer&>(*pCPUBuffer);
	}

	ConstantBuffer& IInstanceBuffer::GetConstantBuffer()
	{
		pCPUBuffer->SetDirty();
		return static_cast<ConstantBuffer&>(*pCPUBuffer);
	}

	std::string_view IInstanceBuffer::GetPath() const
	{
		return path;
	}

	std::string_view IInstanceBuffer::GetTag() const
	{
		return tag;
	}

}