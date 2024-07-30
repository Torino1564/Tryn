#pragma once
#include "IJob.h"
#include <span>
#include <Core/third/glm/glm.hpp>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class RenderWorker;
	class RenderTask;

	class InstancedJob : public IJob
	{
		friend class RenderQueue;
	public:
		InstancedJob(Drawable* parent, Step* step, std::span<const glm::mat4> transforms, class InstancedModelParent* pParentInstanced);
		void Execute(const IGraphics& gfx) override;
		void Execute(const IContext& ctx) override;
	private:
		struct Data {
			Drawable* pDrawable = nullptr;
			Step* pStep = nullptr;
			std::span<const glm::mat4> transforms = {};
			class InstancedModelParent* instanceParent = nullptr;
		} data = {};
	};
}