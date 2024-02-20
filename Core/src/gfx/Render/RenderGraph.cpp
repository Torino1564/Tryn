#include "RenderGraph.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/Render/SinkAndSource.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	IRenderGraph::IRenderGraph(IGraphics& gfx)
		:
		gfx(gfx),
		pRTV(gfx.GetRenderTargetView()),
		pDSV(gfx.CreateOutputOnlyDepthStencil(gfx.GetDimensions()))
	{
		// Init Sink
		pGlobalSink = MakeUniqueSink(In<IShaderResourceRenderTargetView>("rtv"));

		// Init Source
		auto pSource = MakeUniqueSource(Out<IGenericRenderTargetView>("rtv"), Out<IGenericDepthStencil>("depthStencil"));

		pSource->Set(pRTV, "rtv");
		pSource->Set(pDSV, "depthStencil");

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
			queueKeys[renderQueueID] = (uint16_t)(queues.size() - 1);
			return;
		}
	}
	RenderQueue& IRenderGraph::GetRenderQueueByID(std::string_view ID)
	{
		auto it = queueKeys.find(ID.data());
		if (it != queueKeys.end())
		{
			return queues[(*it).second];
		}

		throw RenderGraphException{ std::format("Did not find a render queue with the ID: {}",ID.data()) };
	}

	RenderQueue& IRenderGraph::GetOrAddRenderQueue(const std::string& renderQueueName)
	{
		auto it = queueKeys.find(renderQueueName);
		if (it != queueKeys.end())
		{
			return queues[(*it).second];
		}
		else
		{
			queues.emplace_back(renderQueueName);
			queueKeys[renderQueueName] = (uint16_t)(queues.size() - 1);
			return queues.back();
		}
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

