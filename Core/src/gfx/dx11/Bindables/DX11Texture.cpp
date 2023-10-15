#include "DX11Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <Core/third/stb_image.h>
#include <core/third/lodepng/lodepng.h>
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx
{
	dx11::DX11Texture::DX11Texture(Graphics& gfx, const std::filesystem::path path, const uint8_t slot)
		:
		gfx(gfx)
	{
		this->path = path.string();
		this->slot = slot;

		unsigned int width, height, numChannels = 4;
		//unsigned char* texture = stbi_load(this->path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

		std::vector<unsigned char> image;

		lodepng::decode(image, width, height, this->path);

		if (numChannels == 4)
		{
			hasAlpha = true;
		}

		//trynass_msg(texture != nullptr, L"The specified file could not be loaded!");

		D3D11_TEXTURE2D_DESC td = {};
		td.Width = width;
		td.Height = height;
		td.MipLevels = 0;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		gfx.GetDevice()->CreateTexture2D(&td, nullptr, &pTexture) >> chk;

		gfx.GetContext()->UpdateSubresource(
			pTexture.Get(), 0u, nullptr, image.data(), width * numChannels, 0u
		);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = -1;

		gfx.GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvd, &pTextureView
		) >> chk;

		gfx.GetContext()->GenerateMips(pTextureView.Get());
	}

	void dx11::DX11Texture::Bind()
	{
		gfx.GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
}