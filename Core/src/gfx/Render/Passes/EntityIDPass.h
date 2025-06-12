#pragma once
#include <Core/src/gfx/render/Passes/RenderQueuePass.h>
#include "Core/src/gfx/Bindables/RenderTargetView.h"
#include "Core/src/gfx/Render/RenderGraph.h"

namespace tryn::gfx
{
	class EntityIDPass : public RenderQueuePass
	{
	public:
		static constexpr auto RESOURCE_NAME = "rtv";
		static constexpr auto PASS_NAME = "EntityIDPass";
		EntityIDPass(IRenderGraph& graph)
			: RenderQueuePass(std::move("EntityIDPass"), graph, std::vector<std::string>{"EntityID"})
		{
			pSink->AddDependency<IRenderTargetView>(RESOURCE_NAME);
			pSource->AddExposure<IRenderTargetView>(RESOURCE_NAME);
		}

		void Execute(const IGraphics& gfx) override
		{
			const auto& rtv = pSink->Get<IRenderTargetView>(RESOURCE_NAME);

			rtv.BindAsRTV();

			// This queue pass knows that the first queue is the entityID one (because it was declared that way on its constructor)
			auto& entityIDQueue = *pQueues[0];

			// All this pass does is run the entityID queue
			entityIDQueue.RunJobs(gfx);
			entityIDQueue.Clear();

			pSource->Set(rtv, RESOURCE_NAME);
		}
	};
}
