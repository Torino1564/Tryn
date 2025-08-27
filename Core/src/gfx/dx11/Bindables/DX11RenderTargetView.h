#pragma once
#include <Core/src/gfx/bindables/RenderTargetView.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/Bindables/DX11DepthStencil.h>

namespace tryn::gfx::dx11
{
	class DX11RenderTargetView : public IRenderTargetView
	{
	public:
		DX11RenderTargetView(const Graphics& gfx, spa::DimensionsI dimensions, uint16_t rtvSlot, TextureFormat format = TextureFormat::B8G8R8A8_UNORM, TextureUsage usage = TextureUsage::GPUOnly, uint16_t textureSlot = 0);
		void Bind() override;
		void Bind(const class IContext& ctx) override;
		ID3D11RenderTargetView* Get() const;
		ID3D11RenderTargetView* const* GetAddressOf() const;
		ID3D11RenderTargetView** GetAddressOf();
		void Clear() const override;
		void Release();

		void RegenerateResources(const spa::DimensionsI dimensions);

	private:
		void RTVCreation(spa::DimensionsI dimensions, TextureFormat format, TextureUsage usage);
		void SRVCreation(uint16_t slot);

		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	};
}
