#pragma once
#include <Core/src/gfx/Render/Passes/RenderQueuePass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>

namespace tryn::gfx
{
	class ForwardLambertianPass : public RenderQueuePass
	{
	public:
		ForwardLambertianPass(class IRenderGraph& graph, std::string name = std::string("lambertian"))
			:
			RenderQueuePass(std::move(name), graph, std::vector<std::string>{"Lambertian"})
		{
			// declare sink and source
			pSink = std::make_unique<SinkType>(In<IGenericRenderTargetView>("rtv"), In<IGenericDepthStencil>("depthStencil"), In<IPxConstantBuffer, Policy::Barrier>("pointLightBuffer"));
			sink = static_cast<SinkType*>(pSink.get());

			pSource = std::make_unique<SourceType>(Out<IGenericRenderTargetView>("rtv"), Out<IGenericDepthStencil>("depthStencil"));
			source = static_cast<SourceType*>(pSource.get());
		}
		void Execute(const IGraphics& gfx) override
		{
			// bind Render Target View
			auto& pRTV = sink->Get<IGenericRenderTargetView>("rtv");
			auto& pDSV = sink->Get<IGenericDepthStencil>("depthStencil");

			pRTV->BindAsRTV(pDSV.get());
			
			// This queue pass knows that the first queue is the lambertian one (because it was declared that way on its constructor)
			auto& lambertianQueue = *pQueues[0];
			
			// All this pass does is run the lambertian queue
			lambertianQueue.RunJobs(gfx);
			lambertianQueue.Clear();

			source->Set(pRTV, "rtv");
			source->Set(pDSV, "depthStencil");
		}
		using SinkType = Sink<In<IGenericRenderTargetView>, In<IGenericDepthStencil>, In<IPxConstantBuffer, Policy::Barrier>>;
		using SourceType = Source<Out<IGenericRenderTargetView>, Out<IGenericDepthStencil>>;

		SinkType* sink;
		SourceType* source;
	};
}