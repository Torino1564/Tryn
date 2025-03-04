#include "TrynPCH.h"
#include "DX11DepthStencil.h"

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	ID3D11DepthStencilView* DX11DepthStencil<Type>::Get() const
	{
		return pDSV.Get();
	}

	template <BufferResourceType Type>
	ID3D11DepthStencilView** DX11DepthStencil<Type>::GetAddressOf()
	{
		return pDSV.GetAddressOf();
	}

	template <BufferResourceType Type>
	DX11DepthStencil<Type>::DX11DepthStencil(const Graphics& gfx, const spa::DimensionsI dimensions,
		ComparissonMode mode) requires (Type == BufferResourceType::OutputOnly):
		gfx(gfx), mode(mode)
	{
		this->type = GraphicAPI::DX11;
		DSVCreation(gfx, dimensions, mode, (Type == BufferResourceType::ShaderResource));
	}

	template <BufferResourceType Type>
	DX11DepthStencil<Type>::DX11DepthStencil(const Graphics& gfx, const spa::DimensionsI dimensions,
		const uint16_t slot, ComparissonMode mode) requires (Type == BufferResourceType::ShaderResource):
		gfx(gfx), mode(mode)
	{
		this->type = GraphicAPI::DX11;
		DSVCreation(gfx, dimensions, mode, (Type == BufferResourceType::ShaderResource));
		SRVCreation(gfx, slot);
	}

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::Bind()
	{
		if constexpr (Type == BufferResourceType::OutputOnly)
		{
			trynass(false).msg(L"Cannot bind a depth stencil view as a shader resource. Use ShaderInputDepthStencil instead.");
		}
		else
		{
			Bind(gfx.GetContextInterface());
		}
	}

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::Bind(const IContext& ctx)
	{
		if constexpr (Type == BufferResourceType::OutputOnly)
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

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::Clear() const
	{
		gfx.GetContext().ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::Release()
	{
		pDSV->Release();
		if constexpr (Type == BufferResourceType::ShaderResource)
		{
			pSRV->Release();
		}
	}

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::RegenerateResource(const spa::DimensionsI dimensions)
	{
		DSVCreation(gfx, dimensions, mode, Type == BufferResourceType::ShaderResource);
		if constexpr (Type == BufferResourceType::ShaderResource)
		{
			SRVCreation(gfx, this->slot);
		}
	}

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::DSVCreation(const Graphics& gfx, const spa::DimensionsI dimensions,
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

	template <BufferResourceType Type>
	void DX11DepthStencil<Type>::SRVCreation(const Graphics& gfx, const uint16_t slot)
		requires (Type == BufferResourceType::ShaderResource)
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

	template class DX11DepthStencil<BufferResourceType::OutputOnly>;
	template class DX11DepthStencil<BufferResourceType::ShaderResource>;
}
