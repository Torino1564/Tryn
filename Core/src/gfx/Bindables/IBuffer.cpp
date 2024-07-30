#include "IBuffer.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	template<BufferType Type, CachingPolicy Policy>
	std::shared_ptr<IVtxConstantBuffer> IBuffer<Type, Policy>::Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl, int slot, std::string tag)
	requires (Type == BufferType::VtxConstant && Policy == CachingPolicy::Caching)
	{
		return gfx::BindablePool::Resolve<IVtxConstantBuffer>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::shared_ptr<IVtxConstantBufferNCach> IBuffer<Type, Policy>::Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl,
		int slot, std::string tag) requires (Type == BufferType::VtxConstant && Policy == CachingPolicy::NonCaching)
	{
		return gfx::BindablePool::Resolve<IVtxConstantBufferNCach>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::shared_ptr<IPxConstantBuffer> IBuffer<Type, Policy>::Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl,
		int slot, std::string tag) requires (Type == BufferType::PxConstant && Policy == CachingPolicy::Caching)
	{
		return gfx::BindablePool::Resolve<IPxConstantBuffer>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::shared_ptr<IPxConstantBufferNCach> IBuffer<Type, Policy>::Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl,
		int slot, std::string tag) requires (Type == BufferType::PxConstant && Policy == CachingPolicy::NonCaching)
	{
		return gfx::BindablePool::Resolve<IPxConstantBufferNCach>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::shared_ptr<IVertexBuffer> IBuffer<Type, Policy>::Resolve(const IGraphics& gfx,
		std::shared_ptr<VertexBuffer> cpuBuffer,
		std::string tag) requires (Type == BufferType::Vertex && Policy == CachingPolicy::Caching)
	{
		return gfx::BindablePool::Resolve<IVertexBuffer>(gfx, cpuBuffer, tag);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::shared_ptr<IIndexBuffer> IBuffer<Type, Policy>::Resolve(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices,
		std::string tag) requires (Type == BufferType::Index && Policy == CachingPolicy::Caching)
	{
		return gfx::BindablePool::Resolve<IIndexBuffer>(gfx, indices, tag);
	}

	template <BufferType Type, CachingPolicy Policy>
	std::string IBuffer<Type, Policy>::GenerateID(const IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer,
		std::string tag) requires (Type == BufferType::Vertex)
	{
			if (tag == "?") return tag;
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#VertexBuffer#" << std::to_string(cpuBuffer->ByteSize()) << "#";

			for (auto& element : cpuBuffer->GetLayout().Elements)
			{
				ss << element.first.GetName() << element.second;
			};
			ss << "#" << tag;

			return ss.str();
	}

	template <BufferType Type, CachingPolicy Policy>
	std::string IBuffer<Type, Policy>::GenerateID(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, std::string tag)
		requires (Type == BufferType::Index)
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

	template <BufferType Type, CachingPolicy Policy>
	std::string IBuffer<Type, Policy>::GenerateID(const IGraphics& gfx, ConstantBufferLayout& cbl, int slot, std::string tag)
		requires (Type == BufferType::PxConstant || Type == BufferType::VtxConstant)
	{
		if (tag == "?") return tag;
		decltype(auto) typeStr = gfx.GetAPIString();
		std::stringstream ss;
		ss << typeStr << "#VtxConstantBuffer#" << slot << '#' << tag;
		return ss.str();
	}

	template <BufferType Type, CachingPolicy Policy>
	void IBuffer<Type, Policy>::Bind()
	{
		throw BufferMissmatchException("Invalid call from interface");
	}

	template <BufferType Type, CachingPolicy Policy>
	void IBuffer<Type, Policy>::Bind(const IContext& context)
	{
		throw BufferMissmatchException("Invalid call from interface");
	}

	template <BufferType Type, CachingPolicy Policy>
	std::vector<std::any> IBuffer<Type, Policy>::GetLayoutFromVB() const
	{
		throw BufferMissmatchException("Invalid call from interface");
	}

	template <BufferType Type, CachingPolicy Policy>
	std::vector<std::any> IBuffer<Type, Policy>::GetSlottedLayoutFromVB(int slot) const
	{
		throw BufferMissmatchException("Invalid call from interface");
	}

	template <BufferType Type, CachingPolicy Policy>
	std::string_view IBuffer<Type, Policy>::GetPath() const
	{
		return path;
	}

	template <BufferType Type, CachingPolicy Policy>
	std::string_view IBuffer<Type, Policy>::GetTag() const
	{
		return tag;
	}

	template <BufferType Type, CachingPolicy Policy>
	VertexLayout& IBuffer<Type, Policy>::GetLayout() requires (Type == BufferType::Vertex)
	{
		return layout;
	}

	template <BufferType Type, CachingPolicy Policy>
	const VertexLayout& IBuffer<Type, Policy>::GetLayout() const requires (Type == BufferType::Vertex)
	{
		return layout;
	}

	template <BufferType Type, CachingPolicy Policy>
	void IBuffer<Type, Policy>::Resize(const std::size_t newSize)
	{
		throw BufferMissmatchException{ "Unsupported Operation! Tried to resize a buffer that doesnt implement the resize operator!" };
	}

	template <BufferType Type, CachingPolicy Policy>
	ElementView IBuffer<Type, Policy>::operator[](std::string id) requires (Type == BufferType::VtxConstant || Type ==
		BufferType::PxConstant)
	{
		return (*std::static_pointer_cast<ConstantBuffer>(pCPUBuffer))[id];
	}

	template <BufferType Type, CachingPolicy Policy>
	void IBuffer<Type, Policy>::Accept_(TechniqueProbe& probe)
	requires (Type == BufferType::VtxConstant || Type ==
		BufferType::PxConstant)
	{
		if (probe.VisitBuffer(*reinterpret_cast<ConstantBuffer*>(pCPUBuffer.get())))
		{
			pCPUBuffer->SetDirty();
		}
	}

	template <BufferType Type, CachingPolicy Policy>
	ConstantBuffer& IBuffer<Type, Policy>::GetCPUBuffer() requires (Type == BufferType::Instance || Type == BufferType::
		VtxConstant || Type == BufferType::PxConstant)
	{
		return reinterpret_cast<ConstantBuffer&>(*pCPUBuffer.get());
	}

	template class IBuffer<BufferType::Index, CachingPolicy::Caching>;
	template class IBuffer<BufferType::Instance, CachingPolicy::Caching>;
	template class IBuffer<BufferType::Vertex, CachingPolicy::Caching>;
	template class IBuffer<BufferType::PxConstant, CachingPolicy::Caching>;
	template class IBuffer<BufferType::VtxConstant, CachingPolicy::Caching>;

	template class IBuffer<BufferType::Index,		CachingPolicy::NonCaching>;
	template class IBuffer<BufferType::Instance,	CachingPolicy::NonCaching>;
	template class IBuffer<BufferType::Vertex,		CachingPolicy::NonCaching>;
	template class IBuffer<BufferType::PxConstant,	CachingPolicy::NonCaching>;
	template class IBuffer<BufferType::VtxConstant, CachingPolicy::NonCaching>;
}