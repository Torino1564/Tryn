#include "RenderGraph.h"
#include "Core/src/gfx/Render/Passes/ClearTargetPass.h"
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/IGraphics.h>

#include "Core/src/gfx/Render/Passes/ForwardLambertianPass.h"
#include "Core/src/gfx/Render/Passes/FullScreenRenderPass.h"
#include <Core/src/gfx/Render/Passes/PointLightBindPass.h>

#include "Core/src/gfx/Render/Passes/EntityIDPass.h"

using namespace tryn::gfx;

TrynGameRenderGraph::TrynGameRenderGraph(IGraphics& gfx)
	:
		IRenderGraph(gfx, false)
	{
		// Init Global Resource

		auto pSource = MakeUniqueSource(Out<IGenericRenderTargetView>("rtv"), Out<IGenericDepthStencil>("depthStencil"), Out<IPxConstantBuffer>("pointLightBuffer"), Out<IShaderResourceRenderTargetView>("OSRtv"), Out<IOutputOnlyRenderTargetView>("EntityIDRTV"));

		pSource->Set(pRTV, "rtv");
		pSource->Set(pDSV, "depthStencil");
		pSource->Set(pPointLightCBuf, "pointLightBuffer");

		pOffScreenBuffer = IShaderResourceRenderTargetView::Resolve(gfx, gfx.GetDimensions(), 0u);
		pSource->Set(pOffScreenBuffer, "OSRtv");

		pEntityIDRTV = IOutputOnlyRenderTargetView::Resolve(gfx, gfx.GetDimensions(), RenderTargetFormat::UINT32_4);
		pSource->Set(pEntityIDRTV, "EntityIDRTV");

		pGlobalSource = std::move(pSource);

		pGlobalSink = MakeUniqueSink(In<IShaderResourceRenderTargetView>("OSRtv"), In<IShaderResourceRenderTargetView>("rtv"));

		AddPass(ClearTargetPass<
					TargetIn<IGenericDepthStencil, "depthStencil">,
					TargetIn<IGenericRenderTargetView, "rtv">,
					TargetIn<IShaderResourceRenderTargetView, "OSRtv">,
					TargetIn<IOutputOnlyRenderTargetView, "EntityIDRTV">
			>("initClear"));
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" }, LinkageParam{ .passName = "initClear", .resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "OSRtv" }, LinkageParam{ .passName = "initClear", .resourceName = "OSRtv" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "depthStencil" }, LinkageParam{ .passName = "initClear", .resourceName = "depthStencil" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "EntityIDRTV" }, LinkageParam{ .passName = "initClear", .resourceName = "EntityIDRTV" });

		AddPass(tryn::gfx::PointLightBindPass(*this));
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" });

		AddPass(ForwardLambertianPass(*this));
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "OSRtv" }, LinkageParam{ .passName = "lambertian", .resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "depthStencil" }, LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" });
		AddLinkage(LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "lambertian", .resourceName = "pointLightBuffer" });

		AddPass(std::move(EntityIDPass(*this, "entityIDPass")));
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "EntityIDRTV" }, LinkageParam{ .passName = "entityIDPass", .resourceName = "rtv" });

		AddPass(std::move(FullscreenRenderPass(*this, "fullscreenPP")));
		AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "rtv" }, LinkageParam{ .passName = "fullscreenPP",.resourceName = "OSBuf" });
		AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "rtv" }, LinkageParam{ .passName = "fullscreenPP",.resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" }, LinkageParam{ .passName = "fullscreenPP",.resourceName = "depthStencil" });

		AddLinkage(LinkageParam{ .passName = "fullscreenPP", .resourceName = "rtv" }, LinkageParam{ .passName = "global",.resourceName = "rtv" });
	}
