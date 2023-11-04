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
		void RunJobsAsync(IGraphics& gfx, ccr::Master* pMaster);
		auto Push(Job job)
		{
			queue.push(job);
		}
	private:
		std::string id;
		std::queue<Job> queue;
	};
}