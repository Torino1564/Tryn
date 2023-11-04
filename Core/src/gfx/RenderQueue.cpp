#include "RenderQueue.h"
#include "Drawable.h"
#include "RenderQueue/Step.h"

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

	void Job::ExecuteAsync(IGraphics& gfx, RenderWorker* worker)
	{
		auto task = [](Drawable* pDrawable, Step* pStep, IGraphics* pGfx, IContext* context ) {
			pDrawable->BindBase(*context);
			pStep->Bind(*pGfx,*context);
			context->DrawIndexed(pDrawable->GetIndexCount());
			};
		worker->SetJob(task,pDrawable,pStep,&gfx,&worker->GetContext());
	}

}

