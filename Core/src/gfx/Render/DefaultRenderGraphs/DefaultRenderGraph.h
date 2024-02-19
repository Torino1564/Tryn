#pragma once
#include <Core/src/gfx/Render/RenderGraph.h>

namespace tryn::gfx
{
	class DefaultRenderGraph : public IRenderGraph
	{
	public:
		DefaultRenderGraph(IGraphics& gfx);
	};
}