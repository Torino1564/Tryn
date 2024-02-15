#pragma once
#include <Core/src/gfx/bindables/DepthStencil.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/utl/EmptyType.h>

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	class DX11DepthStencil : public IDepthStencil<Type>
	{
	public:
		template <BufferResourceType Type = Type>
		DX11DepthStencil(Graphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::OutputOnly)
		:
			gfx(gfx)
		{
			DSVCreation(gfx, dimensions, mode);
		}
		template <BufferResourceType Type = Type>
		DX11DepthStencil(Graphics& gfx, const spa::DimensionsI dimensions, const uint16_t slot, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::ShaderResource)
		:
			gfx(gfx)
		{
			DSVCreation(gfx, dimensions, mode);
			SRVCreation(gfx, slot);
		}
		void Bind() override
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
		void Bind(class IContext& ctx) override
		{
			if constexpr (Type == BufferResourceType::OutputOnly)
			{
				trynass(false).msg(L"Cannot bind a depth stencil view as a shader resource. Use ShaderInputDepthStencil instead.");
			}
			else
			{
				gfx.AssertContextCoherence(ctx);
				auto& dx11ctxt = static_cast<DX11Context&>(ctx).GetContext();

				dx11ctxt.PSGetShaderResources(this->slot, 1, this->pSRV.GetAddressOf());
			}
		}
	private:
		void DSVCreation(Graphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode)
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
			td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			gfx.GetDevice().CreateTexture2D(&td, nullptr, &pDepthStencil) >> chk;

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
			dsvd.Format = DXGI_FORMAT_UNKNOWN;
			dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvd.Texture2D.MipSlice = 0u;
			gfx.GetDevice().CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &pDSV) >> chk;
		}
		void SRVCreation(Graphics& gfx, const uint16_t slot)
		{
			static_assert(Type == BufferResourceType::ShaderResource);

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
				pRes.Get(), &srvDesc, &this->pSRV
			) >> chk;

			this->slot = slot;
		}
		Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
		[[no_unique_address]] std::conditional_t<Type == BufferResourceType::ShaderResource, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, utl::empty_t> pSRV;
	};

	using DX11OutputOnlyDepthStencil = DX11DepthStencil<BufferResourceType::OutputOnly>;
	using DX11ShaderResourceDepthStencil = DX11DepthStencil<BufferResourceType::ShaderResource>;
}