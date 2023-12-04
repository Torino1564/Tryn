#pragma once
#include <Core/src/gfx/IBufferFwd.h>

namespace tryn::gfx::dx11
{
	template<BufferType Type, CachingPolicy Policy>
	class DX11Buffer;

	using DX11VtxConstantBuffer			= DX11Buffer<BufferType::VtxConstant, CachingPolicy::Caching>;
	using DX11VtxConstantBufferNCach	= DX11Buffer<BufferType::VtxConstant, CachingPolicy::NonCaching>;
	using DX11PxConstantBuffer			= DX11Buffer<BufferType::PxConstant, CachingPolicy::Caching>;
	using DX11PxConstantBufferNCach		= DX11Buffer<BufferType::PxConstant, CachingPolicy::NonCaching>;
	using DX11VertexBuffer				= DX11Buffer<BufferType::Vertex, CachingPolicy::Caching>;
}