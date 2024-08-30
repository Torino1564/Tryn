#pragma once
#include "Core/src/gfx/Render/RenderGraph.h"

class TrynGameRenderGraph : public tryn::gfx::IRenderGraph
{
public:
	explicit TrynGameRenderGraph(tryn::gfx::IGraphics& gfx);
private:
	std::shared_ptr<tryn::gfx::IShaderResourceRenderTargetView> pOffScreenBuffer;
};
