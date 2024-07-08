#include "DefaultRenderGraph.h"
#include <Core/src/gfx/Render/Passes/ForwardLambertianPass.h>
#include <Core/src/gfx/Render/Passes/PointLightBindPass.h>

namespace tryn::gfx
{
	DefaultRenderGraph::DefaultRenderGraph(IGraphics& gfx)
		:
		IRenderGraph(gfx)
	{
		AddPass(std::move(PointLightBindPass(*this)));
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" });

		AddPass(std::move(ForwardLambertianPass(*this)));
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" }, LinkageParam{ .passName = "lambertian", .resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "depthStencil" }, LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" });
		AddLinkage(LinkageParam{ .passName = "PointLightBind", .resourceName = "pointLightBuffer" }, LinkageParam{ .passName = "lambertian", .resourceName = "pointLightBuffer" });
		

		AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "rtv" }, LinkageParam{ .passName = "global",.resourceName = "rtv" });
	}
}
