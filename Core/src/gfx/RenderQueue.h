#pragma once
#include <queue>
#include <functional>
#include <Core/src/gfx/RenderWorker.h>
#include <Core/third/glm/mat4x4.hpp>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class BindPointLightTask;
	class PointLight;

	class Job
	{
	friend class RenderQueue;
	public:
		Job(Drawable* parent, Step* step);
		void Execute(IGraphics& gfx);
		void ExecuteAsync(IGraphics& gfx, RenderWorker* worker, std::optional<std::shared_ptr<RenderTask>> taskPtr = std::nullopt);
	private:
		void Execute_(IGraphics& gfx);
		void ExecuteInsanced_(IGraphics& gfx);

		struct Data {
			Drawable* pDrawable = nullptr;
			Step* pStep = nullptr;
		} data;
		struct InstancedData {
			std::span<const glm::mat4> transforms = {};
			class InstancedModelParent* instanceParent = nullptr;
		} instanceData = {};
	public:
		Data& GetData();
		InstancedData& GetInstanceData();
	};

	class RenderQueue
	{
	public:
		RenderQueue(std::string id);
		void RunJobs(IGraphics& gfx);
		void RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers, gfx::PointLight* pPointLight);
		void Push(Job job);
	private:
		void ExecuteBatchAsync(IGraphics& gfx, RenderWorker* worker, std::vector<Job>::iterator, std::vector<Job>::iterator, std::optional<std::shared_ptr<BatchRenderTask>> taskPtr = std::nullopt);
		void BindPointLight(RenderWorker* worker, PointLight* pPointLight, std::optional<std::shared_ptr<BindPointLightTask>> taskPtr = std::nullopt);

	private:
		std::string id;
		std::vector<Job> jobs;
		std::vector<std::shared_ptr<BatchRenderTask>> batchRenderTaskPtrs;
		std::vector<std::shared_ptr<BindPointLightTask>> bindPointLightTaskPtrs;
	};
}