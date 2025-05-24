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
			: RenderQueuePass(std::move(name), graph, std::vector<std::string>{"entityIDQueue"})
		{
			pSink = std::make_unique<SinkType>(In<IGenericRenderTargetView>("rtv"));
			sink = static_cast<SinkType*>(pSink.get());

			pSource = std::make_unique<SourceType>(Out<IGenericRenderTargetView>("rtv"));
			source = static_cast<SourceType*>(pSource.get());
		}

		void Execute(const IGraphics& gfx) override
		{
			auto& pRTV = sink->Get<IGenericRenderTargetView>("rtv");

			pRTV->BindAsRTV();

			// This queue pass knows that the first queue is the entityID one (because it was declared that way on its constructor)
			auto& entityIDQueue = *pQueues[0];

			// All this pass does is run the entityID queue
			entityIDQueue.RunJobs(gfx);
			entityIDQueue.Clear();

			source->Set(pRTV, "rtv");
		}
	private:
		using SinkType = Sink<In<IGenericRenderTargetView>>;
		using SourceType = Source<Out<IGenericRenderTargetView>>;

		SinkType* sink;
		SourceType* source;
	};
}
