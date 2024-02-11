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
		pSource->Set(pRTV, "rtv");

		pGlobalSource = std::move(pSource);
	}
	void IRenderGraph::ExecuteFrame(IGraphics& gfx)
	{
		for (auto& pass : pPasses)
		{
			pass->Execute(gfx);
		}
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
	void IRenderGraph::AddLinkage(LinkageParam&& source_, LinkageParam&& destination_)
	{
		// find both passes
		bool sourceFound = false, destinationFound = false;

		IRenderPass* pSourcePass = nullptr;
		IRenderPass* pDestinationPass = nullptr;

		ISource* pSource = nullptr;
		ISink* pSink = nullptr;

		if (source_.passName == "global")
		{
			sourceFound = true;
			pSource = pGlobalSource.get();
		}
		if (destination_.passName == "global")
		{
			destinationFound = true;
			pSink = pGlobalSink.get();
		}

		// fill the pass pointers
		for (int i = 0; i < pPasses.size(); i++)
		{
			if (!sourceFound && pPasses[i]->GetName() == source_.passName)
			{
				pSourcePass = pPasses[i].get();
				pSource = &pSourcePass->GetSource();
				sourceFound = true;
			}
			if (!destinationFound && pPasses[i]->GetName() == destination_.passName)
			{
				pDestinationPass = pPasses[i].get();
				pSink = &pDestinationPass->GetSink();
				destinationFound = true;
			}
			if (sourceFound && destinationFound)
			{
				break;
			}
		}

		trynass(sourceFound && destinationFound).msg(L"Failed to add the linkage! Reason: could not find the required pair.").ex();

		pSource->Bind(*pSink, source_.resourceName, destination_.resourceName);
	}
}

