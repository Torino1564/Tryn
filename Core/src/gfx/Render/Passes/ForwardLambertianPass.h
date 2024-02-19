#pragma once
#include <Core/src/gfx/Render/Passes/RenderQueuePass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	class ForwardLambertianPass : public RenderQueuePass
	{
	public:
		ForwardLambertianPass(std::string name = std::string("lambertian"))
			:
			RenderQueuePass(std::move(name))
		{
			// declare sink and source
			pSink = std::make_unique<SinkType>(In<IGenericRenderTargetView>("rtv"), In<IGenericDepthStencil>("depthStencil"));
			pSource = std::make_unique<SourceType>(Out<IGenericRenderTargetView>("rtv"), Out<IGenericDepthStencil>("depthStencil"));

			// declare queues to utilize
			queueNames.push_back("Lambertian");
		}
		void Execute(IGraphics& gfx) override
		{
			// bind Render Target View
			auto& concreteSink = *reinterpret_cast<SinkType*>(pSink.get());
			auto& pRTV = concreteSink.Get<IGenericRenderTargetView>("rtv");
			auto& pDSV = concreteSink.Get<IGenericDepthStencil>("rtv");

			pRTV->BindAsRTV(pDSV.get());
			
			// This queue pass knows that the first queue is the lambertian one (because it was declared that way on its constructor)
			auto& lambertianQueue = *pQueues[0];
			
			// All this pass does is run the lambertian queue
			lambertianQueue.RunJobs(gfx);
			lambertianQueue.Clear();
		}
		using SinkType = Sink<In<IGenericRenderTargetView>, In<IGenericDepthStencil>>;
		using SourceType = Source<Out<IGenericRenderTargetView>, Out<IGenericDepthStencil>>;
	};
}