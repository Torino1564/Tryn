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

		// DSV Creation
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsd.DepthFunc = D3D11_COMPARISON_LESS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		gfx.GetDevice().CreateDepthStencilState(&dsd, &pDSState) >> chk;
		gfx.GetContext().OMSetDepthStencilState(pDSState.Get(), 1u);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC td = {};
		td.Height = dimensions.height;
		td.Width = dimensions.width;
		td.MipLevels = 1u;
		td.ArraySize = 1u;
		td.Format = DXGI_FORMAT_D32_FLOAT;
		td.SampleDesc.Count = 1u;
		td.SampleDesc.Quality = 0u;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		gfx.GetDevice().CreateTexture2D(&td, nullptr, &pDepthStencil) >> chk;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = DXGI_FORMAT_UNKNOWN;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0u;
		gfx.GetDevice().CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &pDSV) >> chk;
	}
	void DX11RenderTargetView::Bind()
	{
		gfx.GetContext().OMSetRenderTargets(1u, pRTV.GetAddressOf(), pDSV.Get());
	}
	void DX11RenderTargetView::Bind(IContext& ctx)
	{
		gfx.AssertContextCoherence(ctx);
		auto& dx11ctxt = static_cast<DX11Context&>(ctx).GetContext();
		dx11ctxt.OMSetRenderTargets(1u, pRTV.GetAddressOf(), pDSV.Get());
	}
}
