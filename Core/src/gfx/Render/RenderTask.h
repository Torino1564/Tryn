#pragma once
#include <Core/src/ccr/Master.h>
#include <vector>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class IGraphics;
	class IContext;
	class IJob;

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

	class BatchRenderTask
	{
	public:
		void operator()();

		struct Params {
			std::vector<IJob*>::iterator begin;
			std::vector<IJob*>::iterator end;
			IContext* pContext;
			IGraphics* pGfx;
		} params = {};
	};
}