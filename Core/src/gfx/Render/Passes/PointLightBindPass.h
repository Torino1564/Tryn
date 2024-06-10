#pragma once
#include <Core/src/gfx/Render/Passes/RenderQueuePass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	class PointLightBindPass : public RenderQueuePass
	{
	public:
		PointLightBindPass(class IRenderGraph& graph, std::string name = std::string("PointLightBind"))
			:
			RenderQueuePass(std::move(name), graph, std::vector<std::string>{"PointLightBind"}), graph{graph}
		{
			// declare sink and source
			pSink = std::make_unique<SinkType>(In<IPxConstantBuffer>("pointLightBuffer"));
			pSource = std::make_unique<SourceType>(Out<IPxConstantBuffer>("pointLightBuffer"));
		}
		void Execute(IGraphics& gfx) override
		{
			// Get resources from sinks
			auto& concreteSink = *reinterpret_cast<SinkType*>(pSink.get());
			auto& pPointLightBuffer = concreteSink.Get<IPxConstantBuffer>("pointLightBuffer");

			// This queue pass knows that the first queue is the PointLightBind one (because it was declared that way on its constructor)
			auto& pointLightBindQueue = *pQueues[0];

			// All this pass does is run the lambertian queue
			pointLightBindQueue.RunJobs(gfx);
			pointLightBindQueue.Clear();
		}
		using SinkType = Sink<In<IPxConstantBuffer>>;
		using SourceType = Source<Out<IPxConstantBuffer>>;
	private:
		IRenderGraph& graph;
	};
}