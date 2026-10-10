#pragma once
#include "Core/src/gfx/Render/RenderGraph.h"

class TrynGameRenderGraph : public tryn::gfx::IRenderGraph
{
public:
	explicit TrynGameRenderGraph(tryn::gfx::IGraphics& gfx);
	void ResizeCallback(tryn::spa::DimensionsI dimensions) override;

private:
	std::shared_ptr<tryn::gfx::IRenderTargetView> pOffScreenBuffer;
	std::shared_ptr<tryn::gfx::IRenderTargetView> pEntityIDRTV;
};
