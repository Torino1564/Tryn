#include "RenderQueue.h"
#include "Drawable.h"
#include "RenderQueue/Step.h"
#include <Core/src/log/Log.h>
#include <Core/src/gfx/RenderTask.h>
#include <Core/src/gfx/PointLight.h>
#include <Core/src/gfx/Model/InstancedModel.h>

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

	void RenderQueue::RunJobsAsync(IGraphics& gfx, ccr::Master& pMaster, std::vector<std::unique_ptr<RenderWorker>>& workers, gfx::PointLight* pPointLight)
	{
		// Defer calls to
		const auto workerCount = pMaster.GetWorkerCount();
		const auto queueSize = jobs.size();
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

	Job::Job(Drawable * parent, Step * step, std::span<const glm::mat4> transforms, InstancedModelParent * pParentInstanced)
		:
		data{parent, step},
		instanceData{transforms, pParentInstanced}
	{
	}

	void Job::Execute(IGraphics& gfx)
	{
		if (instanceData.instanceParent == nullptr)
		{
			Execute_(gfx);
		}
		else
		{
			ExecuteInsanced_(gfx);
		}
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

	Job::Data& Job::GetData()
	{
		return data;
	}
	Job::InstancedData & Job::GetInstanceData()
	{
		return instanceData;
	}
	void Job::Execute_(IGraphics& gfx)
	{
		data.pDrawable->BindBase();
		data.pDrawable->BindTransformCBuf();
		data.pStep->Bind(gfx);
		gfx.DrawIndexed(data.pDrawable->GetIndexCount());
	}
	void Job::ExecuteInsanced_(IGraphics& gfx)
	{
		auto& instanceBuffer = instanceData.instanceParent->RequestInstanceBuffer(data.pDrawable->GetID());
		auto& constantBuffer = instanceBuffer.GetCPUBuffer();
		auto instanceArray = constantBuffer["InstanceArray"];

		if (instanceArray.Node().Size() < instanceData.transforms.size())
		{
			instanceArray.Resize(instanceData.transforms.size() + 10);
		}

		auto updatedInstanceArray = constantBuffer["InstanceArray"];

		memset(constantBuffer.Data(), 0, constantBuffer.ByteSize());

		for (auto i = 0; i < instanceData.transforms.size(); i++)
		{
			updatedInstanceArray[i]["transform"].Get<glm::mat4>() = transpose(instanceData.transforms[i]);
		}
		
		instanceBuffer.Bind();
		data.pDrawable->BindBase();
		data.pStep->Bind(gfx);
		gfx.DrawInstancedIndexed(data.pDrawable->GetIndexCount(), instanceData.transforms.size(), 0u, 0u, 0u);
	}
}