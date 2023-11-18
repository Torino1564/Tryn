#pragma once
#include <queue>
#include <functional>
#include <Core/src/gfx/RenderWorker.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class BindPointLightTask;

	class Job
	{
	friend class RenderQueue;
	public:
		Job(Drawable* parent, Step* step);
		void Execute(IGraphics& gfx);
		void ExecuteAsync(IGraphics& gfx, RenderWorker* worker, std::optional<std::shared_ptr<BindPointLightTask>> taskPtr = std::nullopt);

	private:
		void BindPointLight(RenderWorker* worker, std::optional<std::shared_ptr<RenderTask>> taskPtr = std::nullopt);
		struct Data {
			Drawable* pDrawable;
			Step* pStep;
		} data;
	public:
		Data& GetData();
	};

	class RenderQueue
	{
	public:
		RenderQueue(std::string id);
		void RunJobs(IGraphics& gfx);
		void RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers);
		void Push(Job job);
	private:
		void ExecuteBatchAsync(IGraphics& gfx, RenderWorker* worker, std::vector<Job>::iterator, std::vector<Job>::iterator, std::optional<std::shared_ptr<BatchRenderTask>> taskPtr);
	private:
		std::string id;
		std::vector<Job> jobs;
		std::vector<std::shared_ptr<RenderTask>> renderTaskPtrs;
		std::vector<std::shared_ptr<BatchRenderTask>> batchRenderTaskPtrs;
	};
}