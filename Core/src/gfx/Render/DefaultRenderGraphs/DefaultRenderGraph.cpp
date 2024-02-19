#include "DefaultRenderGraph.h"
#include <Core/src/gfx/Render/Passes/ForwardLambertianPass.h>

namespace tryn::gfx
{
	DefaultRenderGraph::DefaultRenderGraph(IGraphics& gfx)
		:
		IRenderGraph(gfx)
	{
		AddPass<ForwardLambertianPass>("lambertian");
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" }, LinkageParam{ .passName = "lambertian", .resourceName = "rtv" });
		AddLinkage(LinkageParam{ .passName = "global", .resourceName = "depthStencil" }, LinkageParam{ .passName = "lambertian", .resourceName = "depthStencil" });

		AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "rtv" }, LinkageParam{ .passName = "global",.resourceName = "rtv" });
	}
}
