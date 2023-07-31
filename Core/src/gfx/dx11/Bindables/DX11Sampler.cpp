#include "DX11Sampler.h"
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11Sampler::DX11Sampler( Graphics& gfx, SamplerType type, bool reflect, int slot)
		:
		gfx(gfx)
	{
		this->type = type;
		this->reflect = reflect;
		this->slot = slot;

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = [type]() {
			switch (type)
			{
			case SamplerType::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
			case SamplerType::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
			default:
			case SamplerType::Bilinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}();

		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

		gfx.GetDevice()->CreateSamplerState(&samplerDesc, &pSamplerState) >> chk;
	}
	void DX11Sampler::Bind()
	{
		gfx.GetContext()->PSSetSamplers((UINT)slot, 1u, pSamplerState.GetAddressOf());
	}
}