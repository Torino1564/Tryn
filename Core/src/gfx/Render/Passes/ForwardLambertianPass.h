#pragma once
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	class ForwardLambertianPass : public IRenderPass
	{
	public:
		ForwardLambertianPass()
		{
			// declare sink and source
			pSink = std::make_unique<SinkType>(In<IRenderTargetView>("rtv"));
			pSource = std::make_unique<SourceType>(Out<IRenderTargetView>("rtv"));

			// declare queues to utilize
			queueNames.push_back("Lambertian");
		}
		void Execute(IGraphics& gfx) override
		{
			// bind Render Target View
			auto& concreteSink = *reinterpret_cast<SinkType*>(pSink.get());
			auto pRTV = concreteSink.Get<IRenderTargetView>("rtv");
			pRTV->Bind();
			
			// This queue pass knows that the first queue is the lambertian one (because it was declared that way on its constructor)
			auto& lambertianQueue = *pQueues[0];
			
			// All this pass does is run the lambertian queue
			lambertianQueue.RunJobs(gfx);
		}
		using SinkType = Sink<In<IRenderTargetView>>;
		using SourceType = Source<Out<IRenderTargetView>>;
	};
}