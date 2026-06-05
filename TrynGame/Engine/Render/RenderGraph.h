#pragma once
#include "Core/src/gfx/Render/RenderGraph.h"

namespace tryn::gfx
{
	class IRenderTargetView;
	class IGraphics;

	class TrynGameRenderGraph : public IRenderGraph
	{
	public:
		explicit TrynGameRenderGraph(IGraphics& gfx);
	private:
		std::shared_ptr<IRenderTargetView> pOffScreenBuffer;
		std::shared_ptr<IRenderTargetView> pEntityIDRTV;
	};
}


