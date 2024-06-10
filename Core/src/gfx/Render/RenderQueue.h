#pragma once
#include <queue>
#include <functional>
#include <Core/src/gfx/Render/RenderWorker.h>
#include <Core/third/glm/mat4x4.hpp>
#include <span>
#include <Core/src/utl/AnyVector.h>
#include <Core/src/gfx/Render/Jobs/IJob.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class BindPointLightTask;
	class PointLight;

	class RenderQueue
	{
	public:
		RenderQueue(std::string id);
		void RunJobs(IGraphics& gfx);
		void RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers, gfx::PointLight* pPointLight);
		void Clear();
		std::uint16_t GetNumberOfJobs() const;
		void Push(IJob* pJob);
		template <typename Job>
		void Push(Job&& job)
		{
			anyVector.PushBack(std::forward<Job&&>(job));
		}
		template <typename Job, typename... Args>
		void Push(Args&&...args)
		{
			anyVector.PushBack(std::move(Job(std::forward<Args>(args)..., anyVector.Size())));
		}
		utl::AnyVector& GetAnyVector();
	private:
		void ExecuteBatchAsync(IGraphics& gfx, RenderWorker* worker, std::vector<IJob*>::iterator, std::vector<IJob*>::iterator, std::optional<std::shared_ptr<BatchRenderTask>> taskPtr = std::nullopt);
		void BindPointLight(RenderWorker* worker, PointLight* pPointLight, std::optional<std::shared_ptr<BindPointLightTask>> taskPtr = std::nullopt);

	private:
		// data
		std::string id;
		std::vector<IJob*> pJobs;
		utl::AnyVector anyVector;
		// multithreaded stuff (will go away)
		std::vector<std::shared_ptr<BatchRenderTask>> batchRenderTaskPtrs;
		std::vector<std::shared_ptr<BindPointLightTask>> bindPointLightTaskPtrs;
	};
}