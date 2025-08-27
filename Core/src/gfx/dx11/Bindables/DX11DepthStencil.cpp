
#include "DX11DepthStencil.h"

namespace tryn::gfx::dx11
{
	ID3D11DepthStencilView* DX11DepthStencil::Get() const
	{
		return pDSV.Get();
	}

	ID3D11DepthStencilView** DX11DepthStencil::GetAddressOf()
	{
		return pDSV.GetAddressOf();
	}

	DX11DepthStencil::DX11DepthStencil(const Graphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, ComparissonMode mode)
	:
		mode(mode), gfx(gfx)
	{
		this->shaderResource = shaderResource;
		this->slot = slot.value_or(0);
		this->type = GraphicAPI::DX11;
		DSVCreation(gfx, dimensions, mode, (shaderResource));
		if (shaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}

	void DX11DepthStencil::Bind()
	{
		if (!shaderResource)
		{
			trynass(false).msg(L"Cannot bind a depth stencil view as a shader resource. Use ShaderInputDepthStencil instead.");
		}
		else
		{
			Bind(gfx.GetContextInterface());
		}
	}

	void DX11DepthStencil::Bind(const IContext& ctx)
	{
		if (!shaderResource)
		{
			trynass(false).msg(L"Cannot bind a depth stencil view as a shader resource. Use ShaderInputDepthStencil instead.");
		}
		else
		{
			gfx.AssertContextCoherence(ctx);
			auto& dx11ctxt = static_cast<const DX11Context&>(ctx).GetContext();

			dx11ctxt.PSGetShaderResources(this->slot, 1, this->pSRV.GetAddressOf());
		}
	}

	void DX11DepthStencil::Clear() const
	{
		gfx.GetContext().ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void DX11DepthStencil::Release() const
	{
		pDSV->Release();
		if (shaderResource)
		{
			pSRV->Release();
		}
	}

	void DX11DepthStencil::RegenerateResource(const spa::DimensionsI dimensions)
	{
		DSVCreation(gfx, dimensions, mode, shaderResource);
		if (shaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}

	void DX11DepthStencil::DSVCreation(const Graphics& gfx, const spa::DimensionsI dimensions,
		ComparissonMode mode, bool isShaderResource)
	{
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
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | (isShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0);
		gfx.GetDevice().CreateTexture2D(&td, nullptr, pDepthStencil.GetAddressOf()) >> chk;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = DXGI_FORMAT_UNKNOWN;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0u;
		gfx.GetDevice().CreateDepthStencilView(pDepthStencil.Get(), &dsvd, pDSV.GetAddressOf()) >> chk;
	}

	void DX11DepthStencil::SRVCreation(const Graphics& gfx, const uint16_t slot)
	{
		// SRV Creation
		Microsoft::WRL::ComPtr<ID3D11Resource> pRes;
		this->pDSV->GetResource(&pRes);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		gfx.GetDevice().CreateShaderResourceView(
			pRes.Get(), &srvDesc, this->pSRV.GetAddressOf()
		) >> chk;

		this->slot = slot;
	}
}
