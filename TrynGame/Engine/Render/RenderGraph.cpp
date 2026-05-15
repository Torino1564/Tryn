#include "RenderGraph.h"
#include "Core/src/gfx/Render/Passes/ClearTargetPass.h"
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/IGraphics.h>

#include "Core/src/gfx/Render/Passes/ForwardLambertianPass.h"
#include "Core/src/gfx/Render/Passes/FullScreenRenderPass.h"
#include <Core/src/gfx/Render/Passes/PointLightBindPass.h>

#include "Core/src/gfx/Bindables/PixelShader.h"
#include "Core/src/gfx/Render/Passes/EntityIDPass.h"

using namespace tryn::gfx;

TrynGameRenderGraph::TrynGameRenderGraph(IGraphics& gfx)
	:
		IRenderGraph(gfx, false)
{
	// Init Global Resources
	{
		pGlobalSource->AddExposure<IRenderTargetView>("rtv");
		pGlobalSource->AddExposure<IDepthStencil>("depthStencil");
		pGlobalSource->AddExposure<IPxConstantBuffer>("pointLightBuffer");
		pGlobalSource->AddExposure<IRenderTargetView>("OSRtv");
		pGlobalSource->AddExposure<IRenderTargetView>("EntityIDRTV");

		pGlobalSource->Set(*pRTV, "rtv");
		pGlobalSource->Set(*pDSV, "depthStencil");
		pGlobalSource->Set(*pPointLightCBuf, "pointLightBuffer");

		pOffScreenBuffer = IRenderTargetView::Resolve(gfx, gfx.GetDimensions(), 0, TextureFormat::B8G8R8A8_UNORM, 0);
		extraRTVs.push_back(pOffScreenBuffer);
		pGlobalSource->Set(*pOffScreenBuffer, "OSRtv");

		pEntityIDRTV = IRenderTargetView::Resolve(gfx, gfx.GetDimensions(), 0, TextureFormat::R32G32B32A32_UINT);
		pGlobalSource->Set(*pEntityIDRTV, "EntityIDRTV");
		extraRTVs.push_back(pEntityIDRTV);

		pGlobalSink->AddDependency<IRenderTargetView>("OSRtv");
		pGlobalSink->AddDependency<IRenderTargetView>("rtv");
	}

	{
		auto& clearPass = AddPass(ClearTargetPass("initClear"));
		clearPass.AddTarget<IRenderTargetView>("rtv");
		clearPass.AddTarget<IDepthStencil>("depthStencil");
		clearPass.AddTarget<IRenderTargetView>("OSRtv");

		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" }, LinkageParam{ .passName = "initClear", .resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "OSRtv" }, LinkageParam{ .passName = "initClear", .resourceName = "OSRtv" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "depthStencil" }, LinkageParam{ .passName = "initClear", .resourceName = "depthStencil" });
	}

	{
		auto pEntityIDClearPS = IPixelShader::Resolve(gfx,IGraphics::GetShaderRootPath() + std::string{ "EntityID_PS.cso" });
		auto& pass = AddPass(FullscreenRenderPass(*this, "entityIDClearPass", false, false, &pEntityIDClearPS));

		auto cblayout = ConstantBufferLayout();
		cblayout.Append(ConstantBufferLayout::Type::UInt32, "entityID");
		cblayout.Append(ConstantBufferLayout::Type::UInt32, "archetypeID");
		cblayout.Append(ConstantBufferLayout::Type::UInt32, "padding");
		cblayout.Append(ConstantBufferLayout::Type::UInt32, "empty");
		cblayout.Solidify();

		auto pPxCBuffer = IPxConstantBuffer::Resolve(gfx, std::move(cblayout));

		auto& cbuf = pPxCBuffer->GetConstantBuffer();
		cbuf["entityID"] = 0u;
		cbuf["archetypeID"] = 0u;
		cbuf["padding"] = 0u;
		cbuf["empty"] = 1u;

		pass.SetConstantBuffer(pPxCBuffer);

		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "EntityIDRTV" }, LinkageParam{ .passName = "entityIDClearPass", .resourceName = "rtv" });
	}

	{
		AddPass(PointLightBindPass(*this));
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" });
	}

	{
		AddPass(ForwardLambertianPass(*this));
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "OSRtv" }, LinkageParam{ .passName = "lambertian", .resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "depthStencil" }, LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" });
		AddLinkage(LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "lambertian", .resourceName = "pointLightBuffer" });
	}

	{
		AddPass(std::move(EntityIDPass(*this)));
		AddLinkage(LinkageParam{ .passName = "entityIDClearPass", .resourceName = "rtv" }, LinkageParam{ .passName = "EntityIDPass", .resourceName = "rtv" });
	}

	{
		AddPass(std::move(FullscreenRenderPass(*this, "fullscreenPP")));
		AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "rtv" }, LinkageParam{ .passName = "fullscreenPP",.resourceName = "OSBuf" });
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "rtv" }, LinkageParam{ .passName = "fullscreenPP",.resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" }, LinkageParam{ .passName = "fullscreenPP",.resourceName = "depthStencil" });
	}

	AddLinkage(LinkageParam{ .passName = "fullscreenPP", .resourceName = "rtv" }, LinkageParam{ .passName = "global",.resourceName = "rtv" });
}
