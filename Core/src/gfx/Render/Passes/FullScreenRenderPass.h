#pragma once
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>

namespace tryn::gfx
{
	class ISampler;
	class FullscreenRenderPass :public IRenderPass
	{
	public:
		FullscreenRenderPass(class IRenderGraph& renderGraph, std::string name = "FullScreenRenderPass", const std::shared_ptr<IPixelShader>* pPS = nullptr);
		void Execute(const IGraphics& gfx) override;
	private:
		// shaders
		std::shared_ptr<class IPixelShader> pFullscreenPS;
		std::shared_ptr<class IVertexShader> pFullscreenVS;

		// fullscreen geometry
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<class IInputLayout> pInputLayout;
		std::shared_ptr<ISampler> pSamplerState;
		std::shared_ptr<class IPrimitiveTopology> pPTopology;
	};
}