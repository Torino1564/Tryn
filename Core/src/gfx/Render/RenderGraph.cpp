#include "RenderGraph.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/Render/SinkAndSource.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	IRenderGraph::IRenderGraph(IGraphics& gfx)
		:
		gfx(gfx)
	{
		// Init Sink
		pGlobalSink = std::make_unique<Sink<In<IRenderTargetView>>>(In<IRenderTargetView>("rtv"));

		// Init Source and create resources
		auto pSource = std::make_unique<Source<Out<IRenderTargetView>>>(Out<IRenderTargetView>("rtv"));
		pRTV = gfx.CreateRenderTargetView(gfx.GetDimensions());
		pSource->Set(pRTV.get(), "rtv");

		pGlobalSource = std::move(pSource);
	}
	void IRenderGraph::AddCamera(Camera* cam)
	{
		pCameras.push_back(cam);
	}
	void IRenderGraph::AddPointLight(PointLight* pLight)
	{
		pPointLights.push_back(pLight);
	}
	void IRenderGraph::AddRenderQueue(std::string renderQueueID)
	{
		if (queueKeys.find(renderQueueID.data()) != queueKeys.end())
		{
			return;
		}
		else
		{
			queues.emplace_back(renderQueueID);
			queueKeys[renderQueueID] = queues.size() - 1;
			return;
		}
	}
	RenderQueue& IRenderGraph::GetRenderQueueByID(std::string_view ID)
	{
		if (queueKeys.find(ID.data()) != queueKeys.end())
		{
			return queues[queueKeys[ID.data()]];
		}

		throw RenderGraphException{ std::format("Did not find a render queue with the ID: {}",ID.data()) };
	}

	void IRenderGraph::Reset()
	{
		pCameras.clear();
		pPointLights.clear();
		for (auto& queue : queues)
		{
			queue.Clear();
		}
	}
}

