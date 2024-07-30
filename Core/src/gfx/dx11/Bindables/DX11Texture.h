#pragma once
#include <core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Bindables/TextureResource.h>

namespace tryn::gfx::dx11
{
	class DX11Texture : public ITexture
	{
	public:
		DX11Texture(const Graphics& gfx, const std::filesystem::path& path, uint8_t slot);
		void Bind() override;
		void Bind(const IContext&) override;
	private:
		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
