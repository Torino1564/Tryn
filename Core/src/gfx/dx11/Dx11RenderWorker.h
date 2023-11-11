#pragma once
#include <d3d11.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/RenderWorker.h>
#include <Core/src/gfx/dx11/Dx11Context.h>

namespace tryn::gfx::dx11
{
	class DX11RenderWorker : public gfx::RenderWorker
	{
		friend class Graphics;
	public:
		DX11RenderWorker(ccr::Master* pMaster, Graphics& gfx)
		{
			pContext = std::make_unique<DX11Context>();
			pMaster_ = pMaster;
			pGfx = &gfx;
		}
		IContext& GetContext()
		{
			return *pContext;
		}

		void SubmitWork() override
		{
			class SubmitTask : public ccr::Task
			{
			public:
				void Execute() override
				{
					params.pContext->Submit();
					trylog.debug(L"Submitted work");
				}
				struct 
				{
					IContext* pContext;
				} params;
			};
			auto submitTask = std::make_unique<SubmitTask>();
			submitTask->params.pContext = pContext.get();

			SetTask(std::move(submitTask));
		}
	};
}