#pragma once
#include <d3d11.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Render/RenderWorker.h>
#include <Core/src/gfx/dx11/Dx11Context.h>

namespace tryn::gfx::dx11
{
	class DX11RenderWorker : public gfx::RenderWorker
	{
		friend class Graphics;
	public:
		DX11RenderWorker(ccr::Master* pMaster, Graphics& gfx)
		{
			pContext = std::make_unique<DX11Context>(gfx);
			pMaster_ = pMaster;
			pGfx = &gfx;
		}
		IContext& GetContext()
		{
			return *pContext;
		}

		void SubmitWork(IGraphics& gfx) override
		{
			trylog.debug(L"Submitted work");
			pContext->Submit(gfx);
		}
	};
}