#pragma once
#include <Core/src/ccr/Master.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class IGraphics;
	class IContext;

	class RenderTask
	{
	public:
		void operator()();

		struct {
			Drawable* pDrawable;
			Step* pStep;
			IGraphics* pGfx;
			IContext* pContext;
		} params = {};
	};
}