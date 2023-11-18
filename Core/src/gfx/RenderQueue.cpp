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
		for (int i = 0 ; i < jobs.size() ; i++)
		{
			jobs[i].Execute(gfx);
		}
		jobs.clear();
	}

	void RenderQueue::RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers)
	{
		// Defer calls to
		const auto workerCount = pMaster.GetWorkerCount();
		const auto queueSize = jobs.size();
		const auto perWorker = queueSize / workerCount;
		const auto remaining = queueSize % workerCount;

		const auto originalSize = renderTaskPtrs.capacity();
		if (originalSize < queueSize)
		{
			renderTaskPtrs.reserve(static_cast<std::size_t>(queueSize * 1.5));
			for (int i = 0; i < (queueSize * 1.5) - originalSize - 1; i++)
			{
				renderTaskPtrs.push_back(std::make_shared<RenderTask>());
			}
		}

		const auto originalSizeBatch = batchRenderTaskPtrs.capacity();
		if (originalSizeBatch < workerCount)
		{
			batchRenderTaskPtrs.reserve(workerCount);
			{
				for (int i = 0; i < workerCount - originalSizeBatch; i++)
				{
					batchRenderTaskPtrs.push_back(std::make_shared<BatchRenderTask>());
				}
			}
		}

		bool first = true;

		for (int i = 0, taskIndex = 0; i < workerCount; i++)
		{
			auto itFirst = jobs.begin() + (perWorker * i);
			auto itEnd = itFirst + perWorker;

			if (first)
			{
				itEnd += remaining;
				first = false;
			}
			
			ExecuteBatchAsync(gfx, workers[i].get(), itFirst, itEnd, batchRenderTaskPtrs[i]);
		}
		pMaster.WaitForWorkers();

		// Execute calls in the main thread
		for (auto& worker : workers)
		{
			worker->SubmitWork(gfx);
		}
		jobs.clear();
	}

	void RenderQueue::Push(Job job)
	{
		jobs.push_back(job);
	}

	Job::Job(Drawable* parent, Step* step)
		:
		data{ parent,step }
	{}

	void Job::Execute(IGraphics& gfx)
	{
		data.pDrawable->BindBase();
		data.pStep->Bind(gfx);
		gfx.DrawIndexed(data.pDrawable->GetIndexCount());
	}

	void Job::ExecuteAsync(IGraphics& gfx, RenderWorker* worker, std::optional<std::shared_ptr<RenderTask>> taskPtr)
	{
		auto renderTask = taskPtr.value_or(std::make_unique<RenderTask>());
		renderTask->params.pContext = &worker->GetContext();
		renderTask->params.pDrawable = this->data.pDrawable;
		renderTask->params.pStep = this->data.pStep;
		renderTask->params.pGfx = &gfx;

		worker->AddTask(std::move(renderTask));
	}

	void RenderQueue::ExecuteBatchAsync(IGraphics& gfx, RenderWorker* worker, std::vector<Job>::iterator beginIt, std::vector<Job>::iterator endIt, std::optional<std::shared_ptr<BatchRenderTask>> taskPtr)
	{
		auto batchRenderTask = taskPtr.value_or(std::make_unique<BatchRenderTask>());
		batchRenderTask->params.begin = beginIt;
		batchRenderTask->params.end = endIt;
		batchRenderTask->params.pContext = &worker->GetContext();
		batchRenderTask->params.pGfx = &gfx;

		worker->AddTask(std::move(batchRenderTask));
	}

	Job::Data& Job::GetData()
	{
		return data;
	}

}

