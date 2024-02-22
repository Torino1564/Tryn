#pragma once
#include <Core/src/gfx/Bindables/BufferResourceType.h>

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	class DX11RenderTargetView;

	using DX11OutputOnlyRenderTargetView = DX11RenderTargetView<BufferResourceType::OutputOnly>;
	using DX11ShaderResourceRenderTargetView = DX11RenderTargetView<BufferResourceType::ShaderResource>;

	template <BufferResourceType Type>
	class DX11DepthStencil;

	using DX11OutputOnlyDepthStencil = DX11DepthStencil<BufferResourceType::OutputOnly>;
	using DX11ShaderResourceDepthStencil = DX11DepthStencil<BufferResourceType::ShaderResource>;
}