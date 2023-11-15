#include "RenderQueue.h"
#include "Drawable.h"
#include "RenderQueue/Step.h"
#include <Core/src/log/Log.h>
#include <Core/src/gfx/RenderTask.h>

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

	void RenderQueue::RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers)
	{
		// Defer calls to
		const auto workerCount = pMaster.GetWorkerCount();
		const auto queueSize = queue.size();
		for (int i = 0; i < queueSize - workerCount; i += workerCount)
		{
			for (int j = 0; j < workerCount; j++, queue.pop())
			{
				auto job = queue.front();
				job.ExecuteAsync(gfx, workers[j].get());
			}
			pMaster.WaitForWorkers();
		}
		int workerIndex = 0;
		while (!queue.empty())
		{
			auto job = queue.front();
			job.ExecuteAsync(gfx, workers[workerIndex++].get());
			queue.pop();
		}
		pMaster.WaitForWorkers(workerIndex);

		// Execute calls in the main thread
		for (auto& worker : workers)
		{
			worker->SubmitWork(gfx);
		}
	}

	void RenderQueue::Push(Job job)
	{
		queue.push(job);
	}

	Job::Job(Drawable* parent, Step* step)
		:
		pDrawable(parent), pStep(step)
	{}

	void Job::Execute(IGraphics& gfx)
	{
		pDrawable->BindBase();
		pStep->Bind(gfx);
		gfx.DrawIndexed(pDrawable->GetIndexCount());
	}

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

