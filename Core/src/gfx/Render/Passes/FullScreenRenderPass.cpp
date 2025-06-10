#include "TrynPCH.h"
#include "FullScreenRenderPass.h"
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include "Core/src/gfx/Bindables/RenderTargetView.h"
#include "Core/src/gfx/Bindables/Sampler.h"

namespace tryn::gfx
{
	FullscreenRenderPass::FullscreenRenderPass(IRenderGraph& renderGraph, std::string name, const std::shared_ptr<IPixelShader>* pPS)
		:
		IRenderPass(name)
	{
		// declare sink and source
		pSink = std::make_unique<Sink>();

		pSink->AddDependency<IGenericRenderTargetView>("rtv");
		pSink->AddDependency<IShaderResourceRenderTargetView>("OSBuf");
		pSink->AddDependency<IGenericDepthStencil>("depthStencil");

		pSource = std::make_unique<Source>();
		pSource->AddExposure<IGenericRenderTargetView>("rtv");
		pSource->AddExposure<IGenericDepthStencil>("depthStencil");

		// Setup Fullscreen Geometry
		VertexLayout vtxLayout;
		vtxLayout.AppendElement(VertexLayout::VertexElement::Position2D);
		
		VertexBuffer vtxBuffer(vtxLayout);
		vtxBuffer.EmplaceBack(glm::vec2{-1, 1});
		vtxBuffer.EmplaceBack(glm::vec2{1, 1});
		vtxBuffer.EmplaceBack(glm::vec2{-1, -1});
		vtxBuffer.EmplaceBack(glm::vec2{1, -1});
		
		auto pIndexBuffer_ = std::make_shared<IndexBuffer>(std::move(IndexBuffer({ 0, 1, 2, 1, 3, 2})));

		pVertexBuffer = IVertexBuffer::Resolve(renderGraph.Gfx(),std::make_shared<VertexBuffer>(vtxBuffer), "FullScreenGeometry");
		pIndexBuffer = IIndexBuffer::Resolve(renderGraph.Gfx(), pIndexBuffer_, "FullScreenGeometry");

		auto shaderRootPath = IGraphics::GetShaderRootPath();

		std::string shaderRootPathStr(shaderRootPath);

		pFullscreenVS = IVertexShader::Resolve(renderGraph.Gfx(), shaderRootPathStr + "Fullscreen_VS.cso");
		pPS == nullptr ? pFullscreenPS = IPixelShader::Resolve(renderGraph.Gfx(), shaderRootPathStr + "DefaultFullscreen_PS.cso") : pFullscreenPS = *pPS;

		pInputLayout = IInputLayout::Resolve(renderGraph.Gfx(), vtxLayout, *pFullscreenVS);
		pSamplerState = ISampler::Resolve(renderGraph.Gfx());
		pPTopology = IPrimitiveTopology::Resolve(renderGraph.Gfx());
	}

	void FullscreenRenderPass::Execute(const IGraphics& gfx)
	{
		// Bind buffers and render targets
		const auto& pRTV = pSink->Get<IGenericRenderTargetView>("rtv");
		const auto& pDSV = pSink->Get<IGenericDepthStencil>("depthStencil");
		const auto& pOSRtv = pSink->Get<IShaderResourceRenderTargetView>("OSBuf");

		pRTV->BindAsRTV(pDSV.get());
		pOSRtv->Bind();

		// Bind fullscreen Geometry
		pVertexBuffer->Bind();
		pIndexBuffer->Bind();
		pInputLayout->Bind();
		pFullscreenVS->Bind();
		pFullscreenPS->Bind();
		pSamplerState->Bind();
		pPTopology->Bind();
		gfx.DrawIndexed(6);

		pSource->Set(pRTV, "rtv");
		pSource->Set(pDSV, "depthStencil");
	}
}


