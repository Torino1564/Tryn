
#include "DX11RenderTargetView.h"

#include "DX11Texture.h"
#include "Core/src/gfx/Bindables/TextureResource.h"

namespace tryn::gfx::dx11
{
	DX11RenderTargetView::DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions, const uint16_t rtvSlot, const TextureFormat format, const TextureUsage usage, const uint16_t textureSlot)
		:
		gfx(gfx)
	{
		slot = rtvSlot;
		pTexture = std::make_unique<DX11Texture>(gfx, dimensions, format, textureSlot, usage);
		RTVCreation(dimensions, format, usage);
		SRVCreation(textureSlot);
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
		const auto format = pTexture->GetFormat();
		const auto textureSlot = pTexture->GetSlot();
		const auto usage = pTexture->GetUsage();
		pTexture.reset();
		pTexture = std::make_unique<DX11Texture>(gfx, dimensions, format, textureSlot, usage);
		RTVCreation(pTexture->GetDimensions(), pTexture->GetFormat(), pTexture->GetUsage());
		SRVCreation(pTexture->GetSlot());
	}

	void DX11RenderTargetView::RTVCreation(const spa::DimensionsI dimensions, TextureFormat format, TextureUsage usage)
	{
		// RTV Creation
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = dimensions.width;
		textureDesc.Height = dimensions.height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = Graphics::MapDXGIFormat(format);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = Graphics::MapD3D11Usage(usage).usage;
		textureDesc.BindFlags = Graphics::MapD3D11Usage(usage).bindFlags;
		textureDesc.CPUAccessFlags = Graphics::MapD3D11Usage(usage).cpuAccessFlags;
		textureDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

		gfx.GetDevice().CreateTexture2D(
			&textureDesc, nullptr, pTexture.GetAddressOf()
		) >> chk;

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		gfx.GetDevice().CreateRenderTargetView(
			pTexture.Get(), &rtvDesc, pRTV.GetAddressOf()
		) >> chk;
	}

	void DX11RenderTargetView::SRVCreation(uint16_t slot)
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

		this->slot = slot;
	}
}
