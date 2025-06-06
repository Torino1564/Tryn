#pragma once
#include "RenderQueue.h"
#include <Core/src/gfx/Render/RenderPass.h>
#include <Core/src/utl/Exception.h>
#include <map>
#include <vector>
#include <Core/src/gfx/RTVDSFwd.h>
#include <Core/src/gfx/ConstantBuffer.h>

namespace tryn::gfx
{
	class IGraphics;
	ZT_EX_DEF(RenderGraphException);

	static constexpr auto MAX_RENDER_QUEUE_NUMBER = 50;

	class Camera;
	class PointLight;
	class IRenderGraph
	{
		friend class PointLightJob;
		friend class PointLightBindPass;

	public:
		IRenderGraph(const IGraphics& gfx, const bool initDefaultSourceAndSinks = true);
		virtual void ExecuteFrame(const IGraphics& gfx);
		virtual ~IRenderGraph() = default;
		void AddCamera(Camera*);
		void AddPointLight(PointLight*);
		void AddRenderQueue(std::string renderQueueID);
		virtual void RunQueues(const IGraphics& gfx) {};
		RenderQueue& GetRenderQueueByID(std::string_view ID);
		RenderQueue& GetOrAddRenderQueue(const std::string& renderQueueName);
		void Reset();
		std::uint16_t GetMaxPointLights() const;
		void ResizePointLightBuffer(const std::uint16_t newSize);
		const IGraphics& Gfx() const;
	protected:
		template <typename Pass>
		Pass& AddPass(Pass&& pass)
		{
			pPasses.emplace_back(std::unique_ptr<Pass>(new Pass(std::forward<Pass>(pass))));
			return *pPasses.back();
		}
		struct LinkageParam
		{
			std::string passName;
			std::string resourceName;
		};
		void AddLinkage(LinkageParam&& source_, LinkageParam&& destination_) const;
		void Finalize();
		std::vector<std::unique_ptr<IRenderPass>> pPasses;
		struct Level
		{
			std::vector<std::uint16_t> passIndices;
		};
		std::vector<Level> levels;
		std::map<std::string, uint16_t> queueKeys;
		static constexpr std::uint16_t maxQueues = 50;
		std::vector<RenderQueue> queues;
		const IGraphics& gfx;

		// Global graph resources
		std::shared_ptr<IGenericRenderTargetView> pRTV;
		std::shared_ptr<IGenericDepthStencil> pDSV;
		std::unique_ptr<ISink> pGlobalSink;
		std::unique_ptr<ISource> pGlobalSource;

		std::vector<PointLight*> pPointLights;
		std::vector<Camera*> pCameras;
		int selectedPointLight = 0;
		int selectedCamera = 0;

		// Point Lights
		std::uint16_t numPointLights = 0;
		std::uint16_t maxPointLights = 10;
		std::shared_ptr<gfx::IPxConstantBuffer> pPointLightCBuf;
		gfx::ConstantBufferLayout::Node pointLightElement;
	};
}