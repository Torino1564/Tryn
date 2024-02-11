#include "DX11RenderTargetView.h"

namespace tryn::gfx::dx11 
{
	DX11RenderTargetView::DX11RenderTargetView(Graphics& gfx, const spa::DimensionsI dimensions)
		:
		gfx(gfx)
	{
		// RTV Creation
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
		gfx.GetSwapChain().GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;
		gfx.GetDevice().CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRTV.ReleaseAndGetAddressOf());
	}
	void DX11RenderTargetView::Bind()
	{
		trynass(false).msg(L"Cannot bind a render target view as a shader resource. Use ShaderInputRenderTargetView instead.");
	}
	void DX11RenderTargetView::Bind(IContext& ctx)
	{
		Bind();
	}
}
