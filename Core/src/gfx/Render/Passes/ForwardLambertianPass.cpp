#include <Core/src/gfx/Render/Passes/ForwardLambertianPass.h>
#include <Core/src/gfx/Bindables/ConstantBufferResource.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/Render/RenderQueue.h>

namespace tryn::gfx
{
	ForwardLambertianPass::ForwardLambertianPass(class IRenderGraph& graph, const std::string& name)
		:
		RenderQueuePass(std::move(name), graph, std::vector<std::string>{"Lambertian"})
	{
		// declare sink and source
		pSink->AddDependency<IRenderTargetView>("rtv");
		pSink->AddDependency<IDepthStencil>("depthStencil");
		pSink->AddDependency<IConstantBufferResource>("pointLightBuffer");

		pSource->AddExposure<IRenderTargetView>("rtv");
		pSource->AddExposure<IDepthStencil>("depthStencil");
	}

	void ForwardLambertianPass::Execute(const IGraphics& gfx)
	{
		// bind Render Target View
		auto& rtv = pSink->Get<IRenderTargetView>("rtv");
		auto& dsv = pSink->Get<IDepthStencil>("depthStencil");

		rtv.SetDepthStencil(dsv);
		rtv.Bind();

		// This queue pass knows that the first queue is the lambertian one (because it was declared that way on its constructor)
		auto& lambertianQueue = *pQueues[0];

		// All this pass does is run the lambertian queue
		lambertianQueue.RunJobs(gfx);
		lambertianQueue.Clear();

		pSource->Set(rtv, "rtv");
		pSource->Set(dsv, "depthStencil");
	}
}