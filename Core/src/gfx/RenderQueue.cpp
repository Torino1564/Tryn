#include "RenderQueue.h"
#include "Drawable.h"
#include "RenderQueue/Step.h"
#include <Core/src/utl/Timer.h>

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
		{
			//utl::Timer<utl::LogTimerCallback> timer("Job BindStage");
			pDrawable->BindBase();
			pStep->Bind(gfx);
		}
		{
			//utl::Timer<utl::LogTimerCallback> timer("Job Draw Call");
			gfx.DrawIndexed(pDrawable->GetIndexCount());
		}
		//trylog.info(L"Finished the Job");
	}
}

