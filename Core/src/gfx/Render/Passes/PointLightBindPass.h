#pragma once
#include <Core/src/gfx/Render/Passes/RenderQueuePass.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>

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
			pSink->AddDependency<IPxConstantBuffer>("pointLightBuffer");

			pSource->AddExposure<IPxConstantBuffer>("pointLightBuffer");
		}
		void Execute(const IGraphics& gfx) override
		{
			// Get resources from sinks
			const auto& pPointLightBuffer = pSink->Get<IPxConstantBuffer>("pointLightBuffer");

			// This queue pass knows that the first queue is the PointLightBind one (because it was declared that way on its constructor)
			auto& pointLightBindQueue = *pQueues[0];

			if (pointLightBindQueue.GetNumberOfJobs() > graph.GetMaxPointLights())
			{
				graph.ResizePointLightBuffer(static_cast<uint16_t>(1.2f * pointLightBindQueue.GetNumberOfJobs()));
			}

			pointLightBindQueue.RunJobs(gfx);
			pointLightBindQueue.Clear();

			graph.pPointLightCBuf->Bind();
			pSource->Set(pPointLightBuffer, "pointLightBuffer");
		}
	private:
		IRenderGraph& graph;
	};
}