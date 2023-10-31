#pragma once
#include <vector>
#include "RenderQueue.h"
#include <map>
#include <Core/src/utl/Exception.h>


namespace tryn::gfx
{
	class IGraphics;
	ZT_EX_DEF(RenderGraphException);

	class Camera;
	class PointLight;
	class IRenderGraph
	{
	public:
		virtual void ExecuteFrame(IGraphics& gfx);
		virtual ~IRenderGraph() = default;
		void AddCamera(Camera*);
		void AddPointLight(PointLight*);
		void AddRenderQueue(std::string renderQueueID);
		RenderQueue& GetRenderQueueByID(std::string_view ID);
		void Reset();
	protected:
		std::map<std::string, uint16_t> queueKeys;
		std::vector<RenderQueue> queues;
		std::vector<PointLight*> pPointLights;
		std::vector<Camera*> pCameras;

		int selectedCamera = 0;
	};
}