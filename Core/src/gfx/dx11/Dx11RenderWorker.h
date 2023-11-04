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
		DX11RenderWorker(ccr::Master* pMaster)
		{
			pMaster_ = pMaster;
		}
		ID3D11DeviceContext& GetContext()
		{
			context.GetContext();
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11CommandList> pCommandList;
		DX11Context context;
	};
}