#include "RenderQueue.h"
#include <Core/src/gfx/Drawable.h>
#include "Step.h"
#include <Core/src/log/Log.h>
#include <Core/src/gfx/Render/RenderTask.h>
#include <Core/src/gfx/PointLight.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	RenderQueue::RenderQueue(std::string id)
		:
		id(id)
	{}
	void RenderQueue::RunJobs(IGraphics& gfx)
	{
		for (int i = 0; i < anyVector.Size(); i++)
		{
			auto pIJob = static_cast<IJob*>(anyVector[i]);
			pIJob->Execute(gfx);
		}
	}

	void RenderQueue::RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers, gfx::PointLight* pPointLight)
	{
		for (int i = 0; i < anyVector.Size(); i++)
		{
			pJobs.push_back(static_cast<IJob*>(anyVector[i]));
		}

		// Defer calls to
		const auto workerCount = pMaster.GetWorkerCount();
		const auto queueSize = pJobs.size();
		const auto perWorker = queueSize / workerCount;
		const auto remaining = queueSize % workerCount;

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

		const auto originalSizePointLightBind = bindPointLightTaskPtrs.capacity();
		if (originalSizePointLightBind < workerCount)
		{
			bindPointLightTaskPtrs.reserve(workerCount);
			{
				for (int i = 0; i < workerCount - originalSizeBatch; i++)
				{
					bindPointLightTaskPtrs.push_back(std::make_shared<BindPointLightTask>());
				}
			}
		}

		for (int i = 0; i < workerCount; i++)
		{
			BindPointLight(workers[i].get(), pPointLight, bindPointLightTaskPtrs[i]);
		}
		pMaster.WaitForWorkers();

		bool first = true;
		for (int i = 0, taskIndex = 0; i < workerCount; i++)
		{
			auto itFirst = pJobs.begin() + (perWorker * i);
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
	}

	void RenderQueue::Clear()
	{
		pJobs.clear();
		anyVector.Clear();
	}

	void RenderQueue::Push(IJob* pJob)
	{
		pJobs.push_back(pJob);
	}

	utl::AnyVector& RenderQueue::GetAnyVector()
	{
		return anyVector;
	}

	void RenderQueue::ExecuteBatchAsync(IGraphics& gfx, RenderWorker* worker, std::vector<IJob*>::iterator beginIt, std::vector<IJob*>::iterator endIt, std::optional<std::shared_ptr<BatchRenderTask>> taskPtr)
	{
		auto batchRenderTask = taskPtr.value_or(std::make_shared<BatchRenderTask>());
		batchRenderTask->params.begin = beginIt;
		batchRenderTask->params.end = endIt;
		batchRenderTask->params.pContext = &worker->GetContext();
		batchRenderTask->params.pGfx = &gfx;

		worker->AddTask(std::move(batchRenderTask));
	}

	void RenderQueue::BindPointLight(RenderWorker* worker, PointLight* pPointLight, std::optional<std::shared_ptr<BindPointLightTask>> taskPtr)
	{
		auto bindPointLightTask = taskPtr.value_or(std::make_unique<BindPointLightTask>());
		bindPointLightTask->params.pLight = pPointLight;
		bindPointLightTask->params.pContext = &worker->GetContext();

		worker->AddTask(std::move(bindPointLightTask));
	}
}