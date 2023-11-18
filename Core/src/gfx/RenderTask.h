#pragma once
#include <Core/src/ccr/Master.h>
#include <vector>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class IGraphics;
	class IContext;
	class Job;

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
			std::vector<Job>::iterator begin;
			std::vector<Job>::iterator end;
			IContext* pContext;
			IGraphics* pGfx;
		} params = {};
	};
}