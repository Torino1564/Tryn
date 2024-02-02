#pragma once
#include <queue>
#include <functional>
#include <Core/src/gfx/Render/RenderWorker.h>
#include <Core/third/glm/mat4x4.hpp>
#include <span>
#include <Core/src/utl/AnyVector.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class BindPointLightTask;
	class PointLight;

	class IJob
	{
		friend class RenderQueue;
	public:
		virtual ~IJob() = default;
		virtual void Execute(IGraphics& gfx) = 0;
		virtual void ExecuteAsync(IContext& gfx) = 0;

		using SysType = IJob;
	};

	class Job : public IJob
	{
	friend class RenderQueue;
	public:
		Job(Drawable* parent, Step* step);
		Job(Drawable* parent, Step* step, std::span<const glm::mat4> transforms, class InstancedModelParent* pParentInstanced);
		void Execute(IGraphics& gfx) override;
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
		void Push(IJob* pJob);
		template <typename Job>
		void Push(Job&& job)
		{
			anyVector.PushBack(std::forward<Job&&>(job));
		}
		utl::AnyVector& GetAnyVector();
	private:
		void ExecuteBatchAsync(IGraphics& gfx, RenderWorker* worker, std::vector<IJob*>::iterator, std::vector<IJob*>::iterator, std::optional<std::shared_ptr<BatchRenderTask>> taskPtr = std::nullopt);
		void BindPointLight(RenderWorker* worker, PointLight* pPointLight, std::optional<std::shared_ptr<BindPointLightTask>> taskPtr = std::nullopt);

	private:
		std::string id;
		std::vector<IJob*> pJobs;
		std::vector<std::shared_ptr<BatchRenderTask>> batchRenderTaskPtrs;
		std::vector<std::shared_ptr<BindPointLightTask>> bindPointLightTaskPtrs;
		utl::AnyVector anyVector;
	};
}