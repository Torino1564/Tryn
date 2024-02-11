#pragma once
#include <Core/src/gfx/bindables/DepthStencil.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	class DX11DepthStencil : public IDepthStencil
	{
	public:
		DX11DepthStencil(Graphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less);
		void Bind() override;
		void Bind(class IContext& ctx) override;
	private:
		Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	};
}