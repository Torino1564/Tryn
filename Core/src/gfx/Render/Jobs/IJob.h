#pragma once

namespace tryn::gfx
{
	class IGraphics;
	class IContext;

	class IJob
	{
		friend class RenderQueue;
	public:
		virtual ~IJob() = default;
		virtual void Execute(IGraphics& gfx) = 0;
		virtual void Execute(IContext& gfx) = 0;
	};
}