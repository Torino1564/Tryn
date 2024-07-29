#pragma once
#include "IJob.h"
#include <Core/third/glm/glm.hpp>
#include <Core/src/gfx/PointLight.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class RenderWorker;
	class RenderTask;

	class PointLightJob : public IJob
	{
		friend class RenderQueue;
	public:
		PointLightJob(const PointLightParameters& parameters, const glm::vec3& position, IRenderGraph& renderGraph, std::uint16_t jobID = 0);
		void Execute(const IGraphics& gfx) override;
		void Execute(IContext& ctx) override;
	private:
		void ExecuteImpl_();
		PointLightParameters const* pParams = {};
		glm::vec3 const* pPosition = nullptr;
		IRenderGraph* pRenderGraph = nullptr;
	};
}