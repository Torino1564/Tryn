
#include "RenderGraph.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/Render/SinkAndSource.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/PointLight.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>

namespace tryn::gfx
{
	IRenderGraph::IRenderGraph(const IGraphics& gfx, const bool initDefaultSourceAndSinks)
		:
		gfx(gfx),
		pRTV(gfx.GetRenderTargetView()),
		pDSV(gfx.GetDepthStencilView())
	{
		pGlobalSink = std::make_unique<Sink>();
		pGlobalSource = std::make_unique<Source>();

		if (initDefaultSourceAndSinks)
		{
			// Init Sink
			pGlobalSink->AddDependency<IRenderTargetView>("rtv");

			// Init Source
			pGlobalSource->AddExposure<IRenderTargetView>("rtv");
			pGlobalSource->AddExposure<IRenderTargetView>("depthStencil");
			pGlobalSource->AddExposure<IPxConstantBuffer>("pointLightBuffer");

			pGlobalSource->Set(pRTV, "rtv");
			pGlobalSource->Set(pDSV, "depthStencil");
			pGlobalSource->Set(pPointLightCBuf, "pointLightBuffer");

		}

		// Point Light buffer init
		
		gfx::ConstantBufferLayout cblayout;
		cblayout.Append(ConstantBufferLayout::UInt32, "numPointLights");
		cblayout.Append(ConstantBufferLayout::Float3, "ambient");
		gfx::ConstantBufferLayout::Node pointLightElement(ConstantBufferLayout::Type::Struct, "pointLightParams");
		pointLightElement.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float4, "position"));
		pointLightElement.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3, "diffuseColor"));
		pointLightElement.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float, "diffuseIntensity"));
		pointLightElement.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float, "constantAtt"));
		pointLightElement.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float, "linearAtt"));
		pointLightElement.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float, "quadraticAtt"));

		this->pointLightElement = pointLightElement;

		cblayout.Append(ConstantBufferLayout::Type::Array, "pointLightArray");
		cblayout["pointLightArray"].Set(pointLightElement, maxPointLights);
		cblayout.Solidify();

		pPointLightCBuf = IPxConstantBuffer::Resolve(gfx, cblayout, 0, "PointLightBuffer");

		// reserve queue space
		queues.reserve(maxQueues);
	}
	void IRenderGraph::ExecuteFrame(const IGraphics& gfx)
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
		if (queueKeys.contains(renderQueueID.data()))
		{
			return;
		}
		else
		{
			queues.emplace_back(renderQueueID);
			trynass(queues.size() < MAX_RENDER_QUEUE_NUMBER).msg(L"Maximum queue number exceeded").ex();
			queueKeys[renderQueueID] = (uint16_t)(queues.size() - 1);
			return;
		}
	}
	RenderQueue& IRenderGraph::GetRenderQueueByID(const std::string_view ID)
	{
		if (const auto it = queueKeys.find(ID.data()); it != queueKeys.end())
		{
			return queues[it->second];
		}

		throw RenderGraphException{ std::format("Did not find a render queue with the ID: {}",ID.data()) };
	}

	RenderQueue& IRenderGraph::GetOrAddRenderQueue(const std::string& renderQueueName)
	{
		auto it = queueKeys.find(renderQueueName);
		if (it != queueKeys.end())
		{
			return queues[it->second];
		}
		else
		{
			queues.emplace_back(renderQueueName);
			trynass(queues.capacity() <= 50).msg(L"Maximum queue number exceeded").ex();
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
	std::uint16_t IRenderGraph::GetMaxPointLights() const
	{
		return maxPointLights;
	}
	void IRenderGraph::ResizePointLightBuffer(const std::uint16_t newSize)
	{
		maxPointLights = newSize;
		pPointLightCBuf->Resize(newSize);
	}

	const IGraphics& IRenderGraph::Gfx() const
	{
		return gfx;
	}

	void IRenderGraph::AddLinkage(LinkageParam&& source_, LinkageParam&& destination_) const
	{
		// find both passes
		bool sourceFound = false, destinationFound = false;

		IRenderPass* pSourcePass = nullptr;
		IRenderPass* pDestinationPass = nullptr;

		Source* pSource = nullptr;
		Sink* pSink = nullptr;

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

		pSink->Bind(*pSource, source_.resourceName, destination_.resourceName);
	}
}

