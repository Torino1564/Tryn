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
		IRenderGraph() {}
		virtual void ExecuteFrame(IGraphics& gfx) {};
		virtual ~IRenderGraph() = default;
		void AddCamera(Camera*);
		void AddPointLight(PointLight*);
		void AddRenderQueue(std::string renderQueueID);
		virtual void RunQueues(IGraphics& gfx) {};
		RenderQueue& GetRenderQueueByID(std::string_view ID);
		template <typename Pass>
		void AddPass()
		{
			pPasses.emplace_back(std::make_unique<Pass>());
		}
		struct LinkageParam
		{
			std::string passName;
			std::string resourceName;
		};
		void AddLinkage(LinkageParam&& source_, LinkageParam&& destination_)
		{
			// find both passes
			bool sourceFound = false, destinationFound = false;

			IRenderPass* pSourcePass = nullptr;
			IRenderPass* pDestinationPass = nullptr;

			// fill the pass pointers
			for (int i = 0; i < pPasses.size(); i++)
			{
				if (!sourceFound && pPasses[i]->GetName() == source_.passName)
				{
					pSourcePass = pPasses[i].get();
					sourceFound = true;
				}
				if (!destinationFound && pPasses[i]->GetName() == destination_.passName)
				{
					pDestinationPass = pPasses[i].get();
					destinationFound = true;
				}
				if (sourceFound && destinationFound)
				{
					break;
				}
			}

			// bind sink to source
			auto& source = pSourcePass->GetSource();
			auto& sink = pDestinationPass->GetSink();

			source.Bind(sink, source_.resourceName, destination_.resourceName);
		}
		void Reset();
		void Finalize();
	protected:
		std::vector<std::unique_ptr<IRenderPass>> pPasses;
		struct Level
		{
			std::vector<std::uint16_t> passIndices;
		};
		std::vector<Level> levels;
		std::map<std::string, uint16_t> queueKeys;
		std::vector<RenderQueue> queues;
		// Graph Resources
		std::unique_ptr<ISink> pSink;
		std::unique_ptr<ISource> pSource;

		std::vector<PointLight*> pPointLights;
		std::vector<Camera*> pCameras;
		IGraphics* gfx = nullptr;
		int selectedPointLight = 0;
		int selectedCamera = 0;
	};
}