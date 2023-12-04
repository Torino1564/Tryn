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
	class IBuffer;

	using IVtxConstantBuffer		= IBuffer<BufferType::VtxConstant, CachingPolicy::Caching>;
	using IVtxConstantBufferNCach	= IBuffer<BufferType::VtxConstant, CachingPolicy::NonCaching>;
	using IPxConstantBuffer			= IBuffer<BufferType::PxConstant, CachingPolicy::Caching>;
	using IPxConstantBufferNCach	= IBuffer<BufferType::PxConstant, CachingPolicy::NonCaching>;
	using IVertexBuffer				= IBuffer<BufferType::Vertex, CachingPolicy::Caching>;
	using IIndexBuffer				= IBuffer<BufferType::Index, CachingPolicy::Caching>;
}