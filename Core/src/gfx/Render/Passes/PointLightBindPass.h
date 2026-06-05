#pragma once
#include <Core/src/gfx/Render/Passes/RenderQueuePass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>

namespace tryn::gfx
{
	class PointLightBindPass : public RenderQueuePass
	{
	public:
		PointLightBindPass(class IRenderGraph& graph, const std::string& name = std::string("PointLightBind"));
		void Execute(const IGraphics& gfx) override;
	private:
		IRenderGraph& graph;
	};
}