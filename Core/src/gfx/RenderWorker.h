#pragma once
#include <Core/src/ccr/Master.h>

namespace tryn::gfx
{
	class Drawable;
	class Step;
	class IGraphics;
	class IContext;

	class RenderWorker : public ccr::Worker
	{
	friend class IGraphics;
	public:
		RenderWorker() : pGfx(nullptr) {}
		virtual ~RenderWorker()
		{
			StopWorking();
		}
		virtual void SubmitWork() = 0;
		IContext& GetContext()
		{
			return *pContext;
		}

	protected:
		IGraphics* pGfx = nullptr;
		std::unique_ptr<IContext> pContext = nullptr;
	};
}