#pragma once
#include <Engine/TrynGameEngine.h>

class TrynGameRenderGraph : public gfx::IRenderGraph
{
public:
	explicit TrynGameRenderGraph(gfx::IGraphics& gfx);
private:
	std::shared_ptr<gfx::IShaderResourceRenderTargetView> pOffScreenBuffer;
};