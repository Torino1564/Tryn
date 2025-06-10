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
			pSink->AddDependency<IGenericRenderTargetView>("rtv");
			pSink->AddDependency<IGenericDepthStencil>("depthStencil");
			pSink->AddDependency<IPxConstantBuffer>("pointLightBuffer");

			pSource->AddExposure<IGenericRenderTargetView>("rtv");
			pSource->AddExposure<IGenericDepthStencil>("depthStencil");
		}
		void Execute(const IGraphics& gfx) override
		{
			// bind Render Target View
			const auto& rtv = pSink->Get<IGenericRenderTargetView>("rtv");
			auto& dsv = pSink->Get<IGenericDepthStencil>("depthStencil");

			rtv.BindAsRTV(&dsv);
			
			// This queue pass knows that the first queue is the lambertian one (because it was declared that way on its constructor)
			auto& lambertianQueue = *pQueues[0];
			
			// All this pass does is run the lambertian queue
			lambertianQueue.RunJobs(gfx);
			lambertianQueue.Clear();

			pSource->Set(rtv, "rtv");
			pSource->Set(dsv, "depthStencil");
		}
	};
}