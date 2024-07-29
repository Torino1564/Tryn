#pragma once
#include <Core/src/gfx/bindables/RenderTargetView.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/gfx/dx11/DX11RTVDSVFwd.h>
#include <Core/src/gfx/dx11/Bindables/DX11DepthStencil.h>

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	class DX11RenderTargetView : public IRenderTargetView<Type>
	{
	public:
		template <BufferResourceType Type = Type>
		DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == BufferResourceType::OutputOnly)
		:
			gfx(gfx)
		{
			this->dimensions = dimensions;
			RTVCreation(gfx, dimensions);
		}

		template <BufferResourceType Type = Type>
		DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource)
		:
			gfx(gfx)
		{
			this->dimensions = dimensions;
			RTVCreation(gfx, dimensions);
			SRVCreation(gfx, slot);
		}
		template <BufferResourceType Type = Type>
		DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture)
			requires (Type == BufferResourceType::OutputOnly)
		:
			gfx(gfx)
		{
			RTVCreation(pTexture);
		}
		void BindAsRTV(IGenericDepthStencil* pDSV) override
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
		void Bind() override
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
		void Bind(class IContext& ctx) override
		{
			if constexpr (Type == BufferResourceType::OutputOnly)
			{
				trynass(false).msg(L"Cannot bind a render target view as a shader resource. Use ShaderInputRenderTargetView instead.");
			}
			else
			{
				gfx.AssertContextCoherence(ctx);
				auto& dx11ctxt = static_cast<DX11Context&>(ctx).GetContext();

				dx11ctxt.PSSetShaderResources(this->slot, 1, this->pSRV.GetAddressOf());
			}
		}
		ID3D11RenderTargetView* Get()
		{
			return pRTV.Get();
		}
		auto GetAddressOf()
		{
			return pRTV.GetAddressOf();
		}
		void Clear() const override
		{
			gfx.GetContext().ClearRenderTargetView(pRTV.Get(), gfx.GetBackgroundColor());
		}
	private:
		void RTVCreation(const Graphics& gfx, const spa::DimensionsI dimensions)
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
		void RTVCreation(ID3D11Texture2D* pTexture_in)
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
		void SRVCreation(const Graphics& gfx, uint16_t slot)
		{
			static_assert(Type == BufferResourceType::ShaderResource);

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
		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
		[[no_unique_address]] std::conditional_t<Type == BufferResourceType::ShaderResource, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, utl::empty_t> pSRV;
	};
}