#pragma once
#include <core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Bindables/TextureResource.h>

struct aiTexture;

namespace tryn::gfx::dx11
{
	class DX11Texture : public ITexture
	{
	public:
		DX11Texture(const Graphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage);
		DX11Texture(const Graphics& gfx, spa::DimensionsI dimensions, TextureFormat format, uint8_t slot, TextureUsage usage);
		void Bind() override;
		void Bind(const IContext&) override;

		ID3D11Texture2D* GetD3D11Texture();
	private:
		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pD3D11Texture;
	};
}
