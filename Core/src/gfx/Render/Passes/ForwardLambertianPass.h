#pragma once
#include <Core/src/gfx/Render/Passes/RenderQueuePass.h>

namespace tryn::gfx
{
	class ForwardLambertianPass : public RenderQueuePass
	{
	public:
		ForwardLambertianPass(class IRenderGraph& graph, std::string name = std::string("lambertian"));
		void Execute(const IGraphics& gfx) override;
	};
}