#pragma once
#include <core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Bindables/Texture.h>

namespace tryn::gfx::dx11
{
	class DX11Texture : public ITexture
	{
	public:
		DX11Texture(Graphics& gfx, std::string_view path, uint8_t slot);
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture;
	};
}
