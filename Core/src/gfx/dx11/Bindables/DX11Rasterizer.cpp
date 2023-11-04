#include "DX11Rasterizer.h"
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11Rasterizer::DX11Rasterizer(Graphics& gfx, bool twoSided)
		:
		gfx(gfx)
	{
		this->twoSided = twoSided;

		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		gfx.GetDevice().CreateRasterizerState(&rasterDesc, &pRasterizer) >> chk;
	}
	void DX11Rasterizer::Bind()
	{
		gfx.GetContext().RSSetState(pRasterizer.Get());
	}
	void DX11Rasterizer::Bind(IContext& context)
	{
		gfx.AssertContextCoherence(context);
		auto& dx11context = static_cast<DX11Context*>(&context)->GetContext();
		dx11context.RSSetState(pRasterizer.Get());
	}
}