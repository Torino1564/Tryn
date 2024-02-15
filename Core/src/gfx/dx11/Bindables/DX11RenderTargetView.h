#pragma once
#include <Core/src/gfx/bindables/RenderTargetView.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/utl/EmptyType.h>

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	class DX11RenderTargetView : public IRenderTargetView<Type>
	{
	public:
		template <BufferResourceType Type = Type>
		DX11RenderTargetView(Graphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == BufferResourceType::OutputOnly)
		:
			gfx(gfx)
		{
			RTVCreation(gfx, dimensions);
		}

		template <BufferResourceType Type = Type>
		DX11RenderTargetView(Graphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource)
		:
			gfx(gfx)
		{
			RTVCreation(gfx, dimensions);
			SRVCreation(gfx, slot);
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

				dx11ctxt.PSGetShaderResources(this->slot, 1, this->pSRV.GetAddressOf());
			}
		}
	private:
		void RTVCreation(Graphics& gfx, const spa::DimensionsI dimensions)
		{
			// RTV Creation
			Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
			gfx.GetSwapChain().GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;
			gfx.GetDevice().CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRTV.ReleaseAndGetAddressOf());
		}
		void SRVCreation(Graphics& gfx, uint16_t slot)
		{
			static_assert(Type == BufferResourceType::ShaderResource);

			// SRV Creation
			Microsoft::WRL::ComPtr<ID3D11Resource> pRes;
			pRTV->GetResource(&pRes);

			// create the resource view on the texture
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			gfx.GetDevice().CreateShaderResourceView(
				pRes.Get(), &srvDesc, &pSRV
			) >> chk;

			this->slot = slot;
		}
		Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
		[[no_unique_address]] std::conditional_t<Type == BufferResourceType::ShaderResource, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, utl::empty_t> pSRV;
	};

	using DX11OutputOnlyRenderTargetView = DX11RenderTargetView<BufferResourceType::OutputOnly>;
	using DX11ShaderResourceRenderTargetView = DX11RenderTargetView<BufferResourceType::ShaderResource>;
}