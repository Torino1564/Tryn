#pragma once
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>

namespace tryn::gfx
{
	class ISampler;
	class IPixelShader;

	class FullscreenRenderPass : public IRenderPass
	{
	public:
		FullscreenRenderPass(class IRenderGraph& renderGraph, const std::string& name = "FullScreenRenderPass", bool bindOSRtv = true, bool bindDepthStencil = true, const std::shared_ptr<IPixelShader>* pPS = nullptr);
		void Execute(const IGraphics& gfx) override;
		void SetConstantBuffer(const std::shared_ptr<IPxConstantBuffer>& pPxConstantBuffer);
	private:
		// bindables
		bool bindOSRtv = true;
		bool binddepthStencil = true;

		// shaders
		std::shared_ptr<IPixelShader> pFullscreenPS;
		std::shared_ptr<class IVertexShader> pFullscreenVS;

		// fullscreen geometry
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<class IInputLayout> pInputLayout;
		std::shared_ptr<ISampler> pSamplerState;
		std::shared_ptr<class IPrimitiveTopology> pPTopology;
		std::shared_ptr<IPxConstantBuffer> pPxConstantBuffer = nullptr;
	};
}