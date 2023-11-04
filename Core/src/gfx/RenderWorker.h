#pragma once
#include <Core/src/ccr/Master.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class IGraphics;

	class RenderWorker : public ccr::Worker<Drawable*,Step*,IGraphics*>
	{
	friend class IGraphics;
	public:
		RenderWorker() : pGfx(nullptr) {}
		virtual ~RenderWorker() = default;

	protected:
		IGraphics* pGfx = nullptr;
	};
}