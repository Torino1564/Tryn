#include "DX11RenderTargetView.h"

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	DX11RenderTargetView<Type>::DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions) requires (
		Type == BufferResourceType::OutputOnly):
		gfx(gfx)
	{
		this->dimensions = dimensions;
		RTVCreation(gfx, dimensions);
	}

	template <BufferResourceType Type>
	DX11RenderTargetView<Type>::DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions,
		uint16_t slot) requires (Type == BufferResourceType::ShaderResource):
		gfx(gfx)
	{
		this->dimensions = dimensions;
		RTVCreation(gfx, dimensions);
		SRVCreation(gfx, slot);
	}

	template <BufferResourceType Type>
	DX11RenderTargetView<Type>::DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture) requires (Type ==
		BufferResourceType::OutputOnly):
		gfx(gfx)
	{
		RTVCreation(pTexture);
	}

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::BindAsRTV(IGenericDepthStencil* pDSV)
	{
		ID3D11DepthStencilView* pDepthStencilView = nullptr;

		if (dynamic_cast<DX11OutputOnlyDepthStencil*>(pDSV))
		{
			pDepthStencilView = static_cast<DX11OutputOnlyDepthStencil*>(pDSV)->Get();
		}
		else if (dynamic_cast<DX11ShaderResourceDepthStencil*>(pDSV))
		{
			pDepthStencilView = static_cast<DX11ShaderResourceDepthStencil*>(pDSV)->Get();
		}
		else
		{
			trylog.error(L"Invalid Depth Stencil View passed to the BindAsRTV function (API type missmatch)");
			return;
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

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::Bind()
	{
		if constexpr (Type == BufferResourceType::OutputOnly)
		{
			trynass(false).msg(L"Cannot bind a render target view as a shader resource. Use ShaderInputRenderTargetView instead.");
		}
		else
		{
			Bind(gfx.GetContextInterface());
		}
	}

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::Bind(const IContext& ctx)
	{
		if constexpr (Type == BufferResourceType::OutputOnly)
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

	template <BufferResourceType Type>
	ID3D11RenderTargetView* DX11RenderTargetView<Type>::Get() const
	{
		return pRTV.Get();
	}

	template <BufferResourceType Type>
	ID3D11RenderTargetView* const* DX11RenderTargetView<Type>::GetAddressOf() const
	{
		return pRTV.GetAddressOf();
	}

	template <BufferResourceType Type>
	ID3D11RenderTargetView** DX11RenderTargetView<Type>::GetAddressOf()
	{
		return pRTV.GetAddressOf();
	}

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::Clear() const
	{
		gfx.GetContext().ClearRenderTargetView(pRTV.Get(), gfx.GetBackgroundColor());
	}

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::RTVCreation(const Graphics& gfx, const spa::DimensionsI dimensions)
	{
		// RTV Creation
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = dimensions.width;
		textureDesc.Height = dimensions.height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // never do we not want to bind offscreen RTs as inputs
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		gfx.GetDevice().CreateTexture2D(
			&textureDesc, nullptr, &pTexture
		) >> chk;

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		gfx.GetDevice().CreateRenderTargetView(
			pTexture.Get(), &rtvDesc, &pRTV
		) >> chk;
	}

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::RTVCreation(ID3D11Texture2D* pTexture_in)
	{
		this->pTexture = {pTexture_in};
		// get information from texture about dimensions
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc(&textureDesc);
		this->dimensions.width = textureDesc.Width;
		this->dimensions.height = textureDesc.Height;

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;

		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

		gfx.GetDevice().CreateRenderTargetView(
			pTexture.Get(), &rtvDesc, &pRTV
		) >> chk;
	}

	template <BufferResourceType Type>
	void DX11RenderTargetView<Type>::SRVCreation(const Graphics& gfx, uint16_t slot)
		requires (Type == BufferResourceType::ShaderResource)
	{
		// SRV Creation
		Microsoft::WRL::ComPtr<ID3D11Resource> pRes;
		pRTV->GetResource(&pRes);

		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc(&textureDesc);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		gfx.GetDevice().CreateShaderResourceView(
			pRes.Get(), &srvDesc, &pSRV
		) >> chk;

		this->slot = slot;
	}

	template class DX11RenderTargetView<BufferResourceType::ShaderResource>;
	template class DX11RenderTargetView<BufferResourceType::OutputOnly>;
}
