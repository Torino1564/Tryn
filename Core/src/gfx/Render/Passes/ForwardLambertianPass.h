#pragma once
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	ZT_DEFINE_RENDER_PASS(ForwardLambertianPass)
	{
	public:
		ForwardLambertianPass()
		{
			// Register source and sink
			pSource = &source;
			pSink = &sink;

			// declare queues to utilize
			queueNames.push_back("Lambertian");
		}
		void Execute(IGraphics& gfx) override
		{
			auto pRTV = sink.Get<IRenderTargetView>("rtv");
			pRTV->Bind();
			
			// This queue pass knows that the first queue is the lambertian one (because it was declared that way on its constructor)
			auto& lambertianQueue = *pQueues[0];
			
			// All this pass does is run the lambertian queue
			lambertianQueue.RunJobs(gfx);
		}
	private:
		ZT_DECLARE_EXPOSURES(Out<IRenderTargetView>("rtv"));
		ZT_DECLARE_DEPENDENCIES(Out<IRenderTargetView>("rtv"));
	};
}