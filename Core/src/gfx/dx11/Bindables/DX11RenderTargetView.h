#pragma once
#include <Core/src/gfx/bindables/RenderTargetView.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/Bindables/DX11DepthStencil.h>

namespace tryn::gfx::dx11
{
	class DX11RenderTargetView : public IRenderTargetView
	{
	public:
		DX11RenderTargetView(const Graphics& gfx, spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format);
		DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture, bool shaderResource, std::optional<uint16_t> slot);
		void BindAsRTV(IDepthStencil* pDSV = nullptr) const override;
		void Bind() override;
		void Bind(const class IContext& ctx) override;
		ID3D11RenderTargetView* Get() const;
		ID3D11RenderTargetView* const* GetAddressOf() const;
		ID3D11RenderTargetView** GetAddressOf();
		void Clear() const override;
		void Release();

		void RegenerateResources(const spa::DimensionsI dimensions);
		void RegenerateResources(ID3D11Texture2D* pTextureIn);

		void FillTexture(const std::shared_ptr<ITexture>&) override;
		void FillTextureRegion(const std::shared_ptr<ITexture>&, uint32_t startX, uint32_t endX, uint32_t startY, uint32_t endY) override;

	private:
		void RTVCreation(const Graphics& gfx, spa::DimensionsI dimensions);
		void RTVCreation(ID3D11Texture2D* pTextureIn);
		void SRVCreation(const Graphics& gfx, uint16_t slot);

		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	};
}
