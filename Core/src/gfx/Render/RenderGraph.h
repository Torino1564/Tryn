#pragma once
#include "RenderQueue.h"
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/utl/Exception.h>
#include <map>
#include <vector>

namespace tryn::gfx
{
	class IGraphics;
	ZT_EX_DEF(RenderGraphException);

	class Camera;
	class PointLight;
	class IRenderGraph
	{
	public:
		IRenderGraph(IGraphics& gfx);
		virtual void ExecuteFrame(IGraphics& gfx) {};
		virtual ~IRenderGraph() = default;
		void AddCamera(Camera*);
		void AddPointLight(PointLight*);
		void AddRenderQueue(std::string renderQueueID);
		virtual void RunQueues(IGraphics& gfx) {};
		RenderQueue& GetRenderQueueByID(std::string_view ID);
		template <typename Pass>
		void AddPass(std::string name)
		{
			pPasses.emplace_back(std::make_unique<Pass>(std::move(name)));
		}
		struct LinkageParam
		{
			std::string passName;
			std::string resourceName;
		};
		void Reset();
	protected:
		void AddLinkage(LinkageParam&& source_, LinkageParam&& destination_)
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
		void Finalize();
		std::vector<std::unique_ptr<IRenderPass>> pPasses;
		struct Level
		{
			std::vector<std::uint16_t> passIndices;
		};
		std::vector<Level> levels;
		std::map<std::string, uint16_t> queueKeys;
		std::vector<RenderQueue> queues;
		IGraphics& gfx;
		// Global graph resources
		std::shared_ptr<class IRenderTargetView> pRTV;
		std::unique_ptr<ISink> pGlobalSink;
		std::unique_ptr<ISource> pGlobalSource;

		std::vector<PointLight*> pPointLights;
		std::vector<Camera*> pCameras;
		int selectedPointLight = 0;
		int selectedCamera = 0;
	};
}