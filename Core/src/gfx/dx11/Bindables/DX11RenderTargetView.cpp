#include "TrynPCH.h"
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
		RTVCreation(gfx, dimensions);
		if (shaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}

	DX11RenderTargetView::DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture, const bool shaderResource, const std::optional<uint16_t> slot)
		:
		gfx(gfx)
	{
		this->shaderResource = shaderResource;
		this->slot = slot.value_or(0);
		RTVCreation(pTexture);
		if (shaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}

	void DX11RenderTargetView::BindAsRTV(IDepthStencil* pDSV) const
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
		vp.Width = (float)this->dimensions.width;
		vp.Height = (float)this->dimensions.height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		gfx.GetContext().RSSetViewports(1u, &vp);
	}

	void DX11RenderTargetView::Bind()
	{
		if (!shaderResource)
		{
			trynass(false).msg(L"Cannot bind a render target view as a shader resource. Use ShaderInputRenderTargetView instead.");
		}
		else
		{
			Bind(gfx.GetContextInterface());
		}
	}

	void DX11RenderTargetView::Bind(const IContext& ctx)
	{
		if (!shaderResource)
		{
			trynass(false).msg(L"Cannot bind a render target view as a shader resource. Use ShaderInputRenderTargetView instead.");
		}
		else
		{
			gfx.AssertContextCoherence(ctx);
			auto& dx11ctxt = static_cast<const DX11Context&>(ctx).GetContext();

			dx11ctxt.PSSetShaderResources(this->slot, 1, this->pSRV.GetAddressOf());
		}
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
		if (shaderResource)
		{
			pSRV->Release();
		}
	}

	void DX11RenderTargetView::RegenerateResources(const spa::DimensionsI dimensions)
	{
		RTVCreation(gfx, dimensions);
		if (shaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}

	void DX11RenderTargetView::RegenerateResources(ID3D11Texture2D* pTextureIn)
	{
		RTVCreation(pTextureIn);
		if (shaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}
	void DX11RenderTargetView::RTVCreation(const Graphics& gfx, const spa::DimensionsI dimensions)
	{
		// RTV Creation
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = dimensions.width;
		textureDesc.Height = dimensions.height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = Graphics::MapDXGIFormat(this->format);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // never do we not want to bind offscreen RTs as inputs
		textureDesc.CPUAccessFlags = 0;
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

	void DX11RenderTargetView::RTVCreation(ID3D11Texture2D* pTextureIn)
	{
		// get information from texture about dimensions
		D3D11_TEXTURE2D_DESC textureDesc;
		pTextureIn->GetDesc(&textureDesc);
		this->dimensions.width = textureDesc.Width;
		this->dimensions.height = textureDesc.Height;

		// TODO: Add format here

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;

		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

		gfx.GetDevice().CreateRenderTargetView(
			pTextureIn, &rtvDesc, pRTV.GetAddressOf()
		) >> chk;
	}

	void DX11RenderTargetView::SRVCreation(const Graphics& gfx, uint16_t slot)
	{
		// SRV Creation
		Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
		this->pRTV->GetResource(&pTexture);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Graphics::MapDXGIFormat(this->format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		gfx.GetDevice().CreateShaderResourceView(
			pTexture.Get(), &srvDesc, pSRV.GetAddressOf()
		) >> chk;

		this->slot = slot;
	}
}
