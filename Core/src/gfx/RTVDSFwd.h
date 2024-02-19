#pragma once
#include <Core/src/gfx/Bindables/BufferResourceType.h>

namespace tryn::gfx
{
	class IGenericRenderTargetView;
	class IGenericDepthStencil;

	template <BufferResourceType Type>
	class IRenderTargetView;

	template <BufferResourceType Type>
	class IDepthStencil;

	using IShaderResourceRenderTargetView = IRenderTargetView<BufferResourceType::ShaderResource>;
	using IOutputOnlyRenderTargetView = IRenderTargetView<BufferResourceType::OutputOnly>;

	using IOutputOnlyDepthStencil = IDepthStencil<BufferResourceType::OutputOnly>;
	using IShaderResourceDepthStencil = IDepthStencil<BufferResourceType::ShaderResource>;
}