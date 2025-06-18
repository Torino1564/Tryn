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
		DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture, bool shaderResource, std::optional<uint16_t> slot);
		void Bind() override;
		void Bind(const class IContext& ctx) override;
		ID3D11RenderTargetView* Get() const;
		ID3D11RenderTargetView* const* GetAddressOf() const;
		ID3D11RenderTargetView** GetAddressOf();
		void Clear() const override;
		void Release();

		void RegenerateResources(const spa::DimensionsI dimensions);
		void RegenerateResources(ID3D11Texture2D* pTextureIn);

	private:
		void RTVCreation(const Graphics& gfx, spa::DimensionsI dimensions);
		void RTVCreation(ID3D11Texture2D* pTextureIn);
		void SRVCreation(const Graphics& gfx, uint16_t slot);

		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	};
}
