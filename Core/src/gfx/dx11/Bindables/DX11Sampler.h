#pragma once
#include <Core/src/gfx/Bindables/Sampler.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11Sampler : public ISampler
	{
	public:
		DX11Sampler(const Graphics& gfx, SamplerType type, bool reflect, int slot);
		void Bind() override;
		void Bind(IContext& context) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
		const Graphics& gfx;
	};
}
