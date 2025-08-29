
#include "DX11Texture.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/src/gfx/dx11/Dx11Context.h>
#include <Core/src/gfx/TexturePool.h>
#include <assimp/texture.h>

namespace tryn::gfx
{
	dx11::DX11Texture::DX11Texture(const Graphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot, TextureUsage usage)
		:
		gfx(gfx)
	{
		this->slot = slot;
		this->pTextureResource = pTexture;
		this->usage = usage;
		this->format = pTexture->GetFormat();
		this->dimensions = { .width = pTexture->GetWidth(), .height = pTexture->GetHeight() };

		D3D11_TEXTURE2D_DESC td = {};
		td.Width = pTextureResource->GetWidth();
		td.Height = pTextureResource->GetHeight();
		td.MipLevels = 0;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pD3D11Texture;
		gfx.GetDevice().CreateTexture2D(&td, nullptr, &pD3D11Texture) >> chk;

		gfx.GetContext().UpdateSubresource(
			pD3D11Texture.Get(), 0u, nullptr, pTextureResource->Data(), pTextureResource->GetRowPitch(), 0u
		);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = -1;

		gfx.GetDevice().CreateShaderResourceView(
			pD3D11Texture.Get(), &srvd, &pTextureView
		) >> chk;

		gfx.GetContext().GenerateMips(pTextureView.Get());
	}

	dx11::DX11Texture::DX11Texture(const Graphics& gfx, spa::DimensionsI dimensions, TextureFormat format, uint8_t slot,
		TextureUsage usage)
			:
		gfx(gfx)
	{
		this->slot = slot;
		this->pTextureResource = nullptr;
		this->usage = usage;
		this->format = format;
		this->dimensions = dimensions;

		D3D11_TEXTURE2D_DESC td = {};
		td.Width = dimensions.width;
		td.Height = dimensions.height;
		td.MipLevels = 0;
		td.ArraySize = 1;
		td.Format = Graphics::MapDXGIFormat(format);
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		gfx.GetDevice().CreateTexture2D(&td, nullptr, &pD3D11Texture) >> chk;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = -1;

		gfx.GetDevice().CreateShaderResourceView(
			pD3D11Texture.Get(), &srvd, &pTextureView
		) >> chk;

		gfx.GetContext().GenerateMips(pTextureView.Get());
	}

	dx11::DX11Texture::DX11Texture(const Graphics& gfx, ID3D11Texture2D* pTexture, TextureFormat format, uint8_t slot, TextureUsage usage)
		: gfx(gfx)
	{
		this->slot = slot;
		this->pTextureResource = nullptr;
		this->usage = usage;
		this->format = format;

		D3D11_TEXTURE2D_DESC td = {};
		pTexture->GetDesc(&td);
		this->dimensions = {.width = (int)td.Width, .height = (int)td.Height};

		if (!(Graphics::MapD3D11Usage(usage).bindFlags & D3D11_BIND_RENDER_TARGET))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = td.Format;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MostDetailedMip = 0;
			srvd.Texture2D.MipLevels = -1;

			gfx.GetDevice().CreateShaderResourceView(
				pD3D11Texture.Get(), &srvd, &pTextureView
			) >> chk;

			gfx.GetContext().GenerateMips(pTextureView.Get());
		}
	}

	void dx11::DX11Texture::Bind()
	{
		gfx.GetContext().PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	void dx11::DX11Texture::Bind(const IContext& ctxt)
	{
		gfx.AssertContextCoherence(ctxt);
		auto& dx11ctxt = static_cast<const DX11Context&>(ctxt).GetContext();
		dx11ctxt.PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	ID3D11Texture2D* dx11::DX11Texture::GetD3D11Texture()
	{
		return pD3D11Texture.Get();
	}
}