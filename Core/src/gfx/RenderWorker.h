#pragma once
#include <Core/src/ccr/Master.h>
#include <Core/src/ccr/Worker.h>
#include <Core/src/gfx/RenderTask.h>

namespace tryn::gfx
{
	class RenderTask;
	class Drawable;
	class Step;
	class IGraphics;
	class IContext;

	class RenderWorker : public ccr::Worker
	{
	friend class IGraphics;
	public:
		RenderWorker() : pGfx(nullptr) {}
		virtual ~RenderWorker()
		{
			Kill();
		}
		virtual void SubmitWork(IGraphics&) = 0;
		virtual void AfterAllTasks() override
		{
			if (!midSubmit)
			{
				pMaster_->SignalDone();
			}
		}
		IContext& GetContext()
		{
			return *pContext;
		}
		RenderTask& GetRenderTask()
		{
			return renderTask;
		}
		void SignalStartSubmitting()
		{
			{
				std::lock_guard lk(mtx_);
				midSubmit = true;
			}
		}
		void SignalEndSubmitting()
		{
			{
				std::lock_guard lk(mtx_);
				midSubmit = false;
			}
			if (!hasWork)
			{
				pMaster_->SignalDone();
			}
		}
	protected:
		RenderTask renderTask;
		IGraphics* pGfx = nullptr;
		std::unique_ptr<IContext> pContext = nullptr;
		bool midSubmit = false;
	};
}