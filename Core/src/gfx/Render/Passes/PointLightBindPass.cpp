#include "PointLightBindPass.h"
#include <Core/src/gfx/Render/RenderQueue.h>
#include <Core/src/gfx/Render/RenderGraph.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Bindables/ConstantBufferResource.h>

namespace tryn::gfx
{
	PointLightBindPass::PointLightBindPass(class IRenderGraph& graph, const std::string& name)
		:
		RenderQueuePass(name, graph, std::vector<std::string>{"PointLightBind"}), graph{ graph }
	{
		// declare sink and source
		pSink->AddDependency<IConstantBufferResource>("pointLightBuffer");

		pSource->AddExposure<IConstantBufferResource>("pointLightBuffer");
	}

	void PointLightBindPass::Execute(const IGraphics& gfx)
	{
		// Get resources from sinks
		const auto& pPointLightBuffer = pSink->Get<IConstantBufferResource>("pointLightBuffer");

		// This queue pass knows that the first queue is the PointLightBind one (because it was declared that way on its constructor)
		auto& pointLightBindQueue = *pQueues[0];

		if (pointLightBindQueue.GetNumberOfJobs() > graph.GetMaxPointLights())
		{
			graph.ResizePointLightBuffer(static_cast<uint16_t>(1.2f * pointLightBindQueue.GetNumberOfJobs()));
		}
		auto& pointLightBuffer = *graph.pPointLightCBuf;
		pointLightBuffer.GetConstantBuffer()["ambient"] = gfx.GetAmbientColor();
		pointLightBuffer.GetConstantBuffer()["numPointLights"] = (uint32_t)pointLightBindQueue.GetNumberOfJobs();

		pointLightBindQueue.RunJobs(gfx);
		pointLightBindQueue.Clear();

		pointLightBuffer.Bind();
		pSource->Set(pPointLightBuffer, "pointLightBuffer");
	}
}