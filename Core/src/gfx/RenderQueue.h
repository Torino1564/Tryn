#pragma once
#include <queue>
#include <functional>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class IGraphics;
	class Job
	{
		friend class RenderQueue;
	public:
		Job(Drawable* parent, Step* step)
			:
			pDrawable(parent), pStep(step)
		{}
		void Execute(IGraphics& gfx);
	private:
		Drawable* pDrawable;
		Step* pStep;
	};
	class RenderQueue
	{
	public:
		RenderQueue(std::string id);
		void RunJobs(IGraphics& gfx);
		auto Push(Job job)
		{
			queue.push(job);
		}
	private:
		std::string id;
		std::queue<Job> queue;
	};
}