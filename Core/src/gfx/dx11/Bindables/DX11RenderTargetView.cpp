
#include "DX11RenderTargetView.h"

#include "DX11Texture.h"
#include "Core/src/gfx/Bindables/TextureResource.h"

namespace tryn::gfx::dx11
{
	DX11RenderTargetView::DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions_, const uint16_t rtvSlot_, const TextureFormat format_, const uint16_t textureSlot_)
		:
		gfx(gfx)
	{
		slot = rtvSlot_;
		textureSlot = textureSlot_;
		dimensions = dimensions_;
		format = format_;
		RTVCreation();
		SRVCreation();
	}

	DX11RenderTargetView::DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture, TextureFormat format, uint16_t rtvSlot, uint16_t textureSlot)
		: gfx(gfx)
	{
		slot = rtvSlot;
		D3D11_TEXTURE2D_DESC td;
		pTexture->GetDesc(&td);

		dimensions = { .width = (int)(td.Width), .height = (int)(td.Height) };
		this->format = format;
		this->pTexture = std::make_shared<DX11Texture>(gfx, pTexture, format, textureSlot, TextureUsage::RenderTarget);
		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = Graphics::MapDXGIFormat(format);
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		gfx.GetDevice().CreateRenderTargetView(
			pTexture, &rtvDesc, pRTV.GetAddressOf()
		) >> chk;
	}

	void DX11RenderTargetView::Bind()
	{
		Bind(gfx.GetContextInterface());
	}

	void DX11RenderTargetView::Bind(const IContext& ctx)
	{
		ID3D11DepthStencilView* pDepthStencilView = nullptr;

		if (pDSV != nullptr)
		{
			trynass(pDSV->GetAPI() == GraphicAPI::DX11).msg(L"Invalid Depth Stencil View passed to the BindAsRTV function (API type missmatch)").ex();
			pDepthStencilView = static_cast<DX11DepthStencil*>(pDSV)->Get();
		}

		gfx.GetContext().OMSetRenderTargets(1u, pRTV.GetAddressOf(), pDepthStencilView);

		// configure viewport
		D3D11_VIEWPORT vp;
		const auto [width, height] = pTexture->GetDimensions();
		vp.Width = static_cast<float>(width);
		vp.Height = static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		gfx.GetContext().RSSetViewports(1u, &vp);
	}

	ID3D11RenderTargetView* DX11RenderTargetView::Get() const
	{
		return pRTV.Get();
	}

	ID3D11RenderTargetView* const* DX11RenderTargetView::GetAddressOf() const
	{
		return pRTV.GetAddressOf();
	}

	ID3D11RenderTargetView** DX11RenderTargetView::GetAddressOf()
	{
		return pRTV.GetAddressOf();
	}

	void DX11RenderTargetView::Clear() const
	{
		gfx.GetContext().ClearRenderTargetView(pRTV.Get(), gfx.GetBackgroundColor());
	}

	void DX11RenderTargetView::Release()
	{
		pRTV->Release();
		pTexture.reset();
	}

	void DX11RenderTargetView::RegenerateResources(const spa::DimensionsI dimensions)
	{
		this->dimensions = dimensions;
		pTexture.reset();
		RTVCreation();
		SRVCreation();
	}

	void DX11RenderTargetView::RegenerateResources(ID3D11Texture2D* pTex)
	{
		pTexture = std::make_shared<DX11Texture>(gfx, pTex, format, slot, TextureUsage::RenderTarget);

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = Graphics::MapDXGIFormat(format);
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		gfx.GetDevice().CreateRenderTargetView(
			pTex, &rtvDesc, pRTV.GetAddressOf()
		) >> chk;
	}

	void DX11RenderTargetView::RTVCreation()
	{
		std::shared_ptr<DX11Texture> pTex = std::make_shared<DX11Texture>(gfx, dimensions, format, slot, TextureUsage::RenderTarget);
		pTexture = pTex;
		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = Graphics::MapDXGIFormat(format);
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		gfx.GetDevice().CreateRenderTargetView(
			pTex->GetD3D11Texture(), &rtvDesc, pRTV.GetAddressOf()
		) >> chk;
	}

	void DX11RenderTargetView::SRVCreation()
	{
		// SRV Creation
		Microsoft::WRL::ComPtr<ID3D11Resource> pTex;
		this->pRTV->GetResource(&pTex);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Graphics::MapDXGIFormat(pTexture->GetFormat());
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		gfx.GetDevice().CreateShaderResourceView(
			pTex.Get(), &srvDesc, pSRV.GetAddressOf()
		) >> chk;
	}
}
