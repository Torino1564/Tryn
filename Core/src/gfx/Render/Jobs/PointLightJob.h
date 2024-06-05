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
		PointLightJob(const gfx::PointLight::Parameters& parameters);
		void Execute(IGraphics& gfx) override;
		void Execute(IContext& ctx) override;
	private:
		PointLight::Parameters params = {};
	};
}