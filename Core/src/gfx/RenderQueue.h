#pragma once
#include <queue>
#include <functional>
#include <Core/src/gfx/RenderWorker.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;

	class Job
	{
	friend class RenderQueue;
	public:
		Job(Drawable* parent, Step* step)
			:
			pDrawable(parent), pStep(step)
		{}
		void Execute(IGraphics& gfx);
		void ExecuteAsync(IGraphics& gfx, RenderWorker* worker);
	private:
		Drawable* pDrawable;
		Step* pStep;
	};

	class RenderQueue
	{
	public:
		RenderQueue(std::string id);
		void RunJobs(IGraphics& gfx);
		void RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers)
		{
			// Defer calls to
			const auto workerCount = pMaster.GetWorkerCount();
			const int queueSize = queue.size();
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
			pMaster.WaitForWorkers();
			// Execute calls in the main thread
			for (auto& worker : workers)
			{
				worker->SubmitWork(gfx);
			}
		}
		auto Push(Job job)
		{
			queue.push(job);
		}
	private:
		std::string id;
		std::queue<Job> queue;
	};
}