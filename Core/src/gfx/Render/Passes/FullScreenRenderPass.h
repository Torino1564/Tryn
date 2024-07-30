#pragma once
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	class ISampler;

	class FullscreenRenderPass :public IRenderPass
	{
	public:
		FullscreenRenderPass(class IRenderGraph& renderGraph, std::string name = "FullScreenRenderPass");

		void Execute(const IGraphics& gfx) override;

		using SinkType = Sink<In<IGenericRenderTargetView>, In<IGenericDepthStencil>, In<IShaderResourceRenderTargetView>>;
		using SourceType = Source<Out<IGenericRenderTargetView>, Out<IGenericDepthStencil>>;
	private:
		// shaders
		std::shared_ptr<class IPixelShader> pFullscreenPS;
		std::shared_ptr<class IVertexShader> pFullscreenVS;

		// fullscreen geometry
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<class IInputLayout> pInputLayout;
		std::shared_ptr<ISampler> pSamplerState;
	};
}