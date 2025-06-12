#include "TrynPCH.h"
#include "DefaultRenderGraph.h"
#include <Core/src/gfx/Render/Passes/ForwardLambertianPass.h>
#include <Core/src/gfx/Render/Passes/PointLightBindPass.h>

#include "Core/src/gfx/Render/Passes/ClearTargetPass.h"

namespace tryn::gfx
{
	DefaultRenderGraph::DefaultRenderGraph(const IGraphics& gfx)
		:
		IRenderGraph(gfx)
	{
		{
			auto& clearPass = AddPass(ClearTargetPass("initClear"));
			clearPass.AddTarget<IDepthStencil>("depthStencil");
			clearPass.AddTarget<IRenderTargetView>("rtv");
			AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" }, LinkageParam{ .passName = "initClear", .resourceName = "rtv" });
			AddLinkage(LinkageParam{ .passName = "global", .resourceName = "depthStencil" }, LinkageParam{ .passName = "initClear", .resourceName = "depthStencil" });
		}

		{
			AddPass(std::move(PointLightBindPass(*this)));
			AddLinkage(LinkageParam{ .passName = "global", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" });
		}

		{
			AddPass(std::move(ForwardLambertianPass(*this)));
			AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "rtv" }, LinkageParam{ .passName = "lambertian", .resourceName = "rtv" });
			AddLinkage(LinkageParam{ .passName = "initClear", .resourceName = "depthStencil" }, LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" });
			AddLinkage(LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "lambertian", .resourceName = "pointLightBuffer" });
		}

		{
			AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "rtv" }, LinkageParam{ .passName = "global",.resourceName = "rtv" });
		}
	}
}
