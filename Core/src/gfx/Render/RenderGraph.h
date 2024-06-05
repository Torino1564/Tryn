#pragma once
#include "RenderQueue.h"
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/utl/Exception.h>
#include <map>
#include <vector>
#include <Core/src/gfx/RTVDSFwd.h>

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
		virtual void ExecuteFrame(IGraphics& gfx);
		virtual ~IRenderGraph() = default;
		void AddCamera(Camera*);
		void AddPointLight(PointLight*);
		void AddRenderQueue(std::string renderQueueID);
		virtual void RunQueues(IGraphics& gfx) {};
		RenderQueue& GetRenderQueueByID(std::string_view ID);
		RenderQueue& GetOrAddRenderQueue(const std::string& renderQueueName);
		uint16_t GetOrAddRenderQueueID(const std::string& renderQueueName);
		std::vector<RenderQueue>& GetQueueVector();
		void Reset();
	protected:
		template <typename Pass>
		void AddPass(Pass&& pass)
		{
			pPasses.emplace_back(std::unique_ptr<Pass>(new Pass(std::forward<Pass>(pass))));
		}
		struct LinkageParam
		{
			std::string passName;
			std::string resourceName;
		};
		void AddLinkage(LinkageParam&& source_, LinkageParam&& destination_);
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
		std::shared_ptr<IGenericRenderTargetView> pRTV;
		std::shared_ptr<IGenericDepthStencil> pDSV;
		std::unique_ptr<ISink> pGlobalSink;
		std::unique_ptr<ISource> pGlobalSource;

		std::vector<PointLight*> pPointLights;
		std::vector<Camera*> pCameras;
		int selectedPointLight = 0;
		int selectedCamera = 0;
	};
}