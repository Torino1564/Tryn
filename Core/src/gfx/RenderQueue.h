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
		Job(Drawable* parent, Step* step);
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
		void RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers);
		void Push(Job job);
	private:
		std::string id;
		std::queue<Job> queue;
	};
}