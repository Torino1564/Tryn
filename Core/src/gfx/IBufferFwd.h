#pragma once

namespace tryn::gfx
{
	enum class CachingPolicy
	{
		Caching,
		NonCaching
	};

	enum class BufferType
	{
		Vertex,
		VtxConstant,
		PxConstant,
		Index,
		Instance
	};
	template<BufferType Type, CachingPolicy Policy = CachingPolicy::Caching>
	class IBufferBase;

	using IVtxConstantBuffer		= IBufferBase<BufferType::VtxConstant, CachingPolicy::Caching>;
	using IVtxConstantBufferNCach	= IBufferBase<BufferType::VtxConstant, CachingPolicy::NonCaching>;
	using IPxConstantBuffer			= IBufferBase<BufferType::PxConstant, CachingPolicy::Caching>;
	using IPxConstantBufferNCach	= IBufferBase<BufferType::PxConstant, CachingPolicy::NonCaching>;
	using IVertexBuffer				= IBufferBase<BufferType::Vertex, CachingPolicy::Caching>;
	using IIndexBuffer				= IBufferBase<BufferType::Index, CachingPolicy::Caching>;
	using IInstanceBuffer			= IBufferBase<BufferType::Instance, CachingPolicy::Caching>;
}