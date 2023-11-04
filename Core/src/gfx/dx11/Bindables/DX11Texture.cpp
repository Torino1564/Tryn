#include "DX11Texture.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/third/stb_image/stb_image.h>
#include <Core/src/utl/String.h>
#include <Core/src/gfx/dx11/Dx11Context.h>

namespace tryn::gfx
{
	dx11::DX11Texture::DX11Texture(Graphics& gfx, const std::filesystem::path path, const uint8_t slot)
		:
		gfx(gfx)
	{
		this->path = path.string();
		this->slot = slot;

		int width, height, numChannels;
		auto texture = stbi_load(this->path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

		if (numChannels == 4)
		{
			hasAlpha = true;
		}

		trynass_msg(texture != nullptr, utl::ToWide(std::format("The specified file could not be loaded! File: {}", this->path.c_str())));

		D3D11_TEXTURE2D_DESC td = {};
		td.Width = width;
		td.Height = height;
		td.MipLevels = 0;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		gfx.GetDevice().CreateTexture2D(&td, nullptr, &pTexture) >> chk;

		gfx.GetContext().UpdateSubresource(
			pTexture.Get(), 0u, nullptr, texture, width * STBI_rgb_alpha, 0u
		);

		stbi_image_free(texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = -1;

		gfx.GetDevice().CreateShaderResourceView(
			pTexture.Get(), &srvd, &pTextureView
		) >> chk;

		gfx.GetContext().GenerateMips(pTextureView.Get());
	}

	void dx11::DX11Texture::Bind()
	{
		gfx.GetContext().PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	void dx11::DX11Texture::Bind(IContext& ctxt)
	{
		gfx.AssertContextCoherence(ctxt);
		auto& dx11ctxt = static_cast<DX11Context&>(ctxt);
		dx11ctxt.GetContext().PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
}