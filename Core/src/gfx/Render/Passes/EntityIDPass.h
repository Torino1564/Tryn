#pragma once
#include <Core/src/gfx/render/Passes/RenderQueuePass.h>
#include "Core/src/gfx/Bindables/RenderTargetView.h"
#include "Core/src/gfx/Render/RenderGraph.h"

namespace tryn::gfx
{
	class EntityIDPass : public RenderQueuePass
	{
	public:
		EntityIDPass(IRenderGraph& graph, std::string name = "EntityIDPass")
			: RenderQueuePass(std::move(name), graph, std::vector<std::string>{"EntityID"})
		{
			pSink->AddDependency<IGenericRenderTargetView>("rtv");
			pSource->AddExposure<IGenericRenderTargetView>("rtv");
		}

		void Execute(const IGraphics& gfx) override
		{
			const auto& rtv = pSink->Get<IGenericRenderTargetView>("rtv");

			rtv.BindAsRTV();

			// This queue pass knows that the first queue is the entityID one (because it was declared that way on its constructor)
			auto& entityIDQueue = *pQueues[0];

			// All this pass does is run the entityID queue
			entityIDQueue.RunJobs(gfx);
			entityIDQueue.Clear();

			pSource->Set(rtv, "rtv");
		}
	};
}
