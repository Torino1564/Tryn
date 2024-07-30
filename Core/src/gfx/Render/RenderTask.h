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
		void operator()() const;

		struct {
			IJob* pJob;
			const IContext* pContext;
		} params = {};
	};

	class BatchRenderTask
	{
	public:
		void operator()() const;

		struct Params {
			std::vector<IJob*>::iterator begin;
			std::vector<IJob*>::iterator end;
			const IContext* pContext;
			const IGraphics* pGfx;
		} params = {};
	};
}