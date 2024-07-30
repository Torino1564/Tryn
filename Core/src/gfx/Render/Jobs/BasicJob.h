#pragma once
#include "IJob.h"
#include <span>
#include <optional>
#include <memory>
#include <Core/third/glm/glm.hpp>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class RenderWorker;
	class RenderTask;

	class BasicJob : public IJob
	{
		friend class RenderQueue;
	public:
		BasicJob(Drawable* parent, Step* step);
		void Execute(const IGraphics& gfx) override;
		void Execute(const IContext& gfx) override;

	private:
		struct Data {
			Drawable* pDrawable = nullptr;
			Step* pStep = nullptr;
		} data;
	public:
		Data& GetData();
	};
}