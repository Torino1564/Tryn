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
	void RenderQueue::RunJobsAsync(IGraphics& gfx, ccr::Master* pMaster)
	{
		while (!queue.empty())
		{

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
		auto task = [](Drawable* pDrawable, Step* pStep, IGraphics* pGfx ) {
			pDrawable->BindBase();
			pStep->Bind(*pGfx);
			pGfx->DrawIndexed(pDrawable->GetIndexCount());
			};
		worker->SetJob(task,pDrawable,pStep,&gfx);
	}

}

