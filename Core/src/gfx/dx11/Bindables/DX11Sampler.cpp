
#include "DX11Sampler.h"
#include <Core/src/gfx/dx11/GraphicsError.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	DX11Sampler::DX11Sampler(const Graphics& gfx, SamplerType type, const bool reflect, const int slot)
		:
		gfx(gfx)
	{
		this->samplerType = type;
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

		gfx.GetDevice().CreateSamplerState(&samplerDesc, &pSamplerState) >> chk;
	}
	void DX11Sampler::Bind()
	{
		gfx.GetContext().PSSetSamplers((UINT)slot, 1u, pSamplerState.GetAddressOf());
	}
	void DX11Sampler::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		auto& dx11context = static_cast<const DX11Context*>(&context)->GetContext();
		dx11context.PSSetSamplers((UINT)slot, 1u, pSamplerState.GetAddressOf());
	}
}