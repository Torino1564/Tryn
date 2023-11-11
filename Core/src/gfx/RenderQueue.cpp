#include "RenderQueue.h"
#include "Drawable.h"
#include "RenderQueue/Step.h"
#include <Core/src/log/Log.h>

namespace tryn::gfx
{
	RenderQueue::RenderQueue(std::string id)
		:
		id(id)
	{}
	void RenderQueue::RunJobs(IGraphics& gfx)
	{
		while (!queue.empty())
		{
			auto& job = queue.front();
			job.Execute(gfx);
			queue.pop();
		}
	}

	void Job::Execute(IGraphics& gfx)
	{
		pDrawable->BindBase();
		pStep->Bind(gfx);
		gfx.DrawIndexed(pDrawable->GetIndexCount());
	}

	class RenderTask : public ccr::Task
	{
	public:
		void Execute() override
		{
			params.pDrawable->BindBase(*params.pContext);
			params.pStep->Bind(*params.pGfx, *params.pContext);
			params.pContext->DrawIndexed(params.pDrawable->GetIndexCount());
		}

		struct {
			Drawable* pDrawable;
			Step* pStep;
			IGraphics* pGfx;
			IContext* pContext;
		} params;
	};

	void Job::ExecuteAsync(IGraphics& gfx, RenderWorker* worker)
	{
		auto renderTask = std::make_unique<RenderTask>();
		renderTask->params.pContext = &worker->GetContext();
		renderTask->params.pDrawable = this->pDrawable;
		renderTask->params.pStep = this->pStep;
		renderTask->params.pGfx = &gfx;

		trylog.debug(L"Submitting Job async");

		worker->SetTask(std::move(renderTask));
	}

}

