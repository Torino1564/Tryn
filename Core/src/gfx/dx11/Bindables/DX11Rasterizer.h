#pragma once
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>

namespace tryn::gfx::dx11
{
	class DX11Rasterizer : public IRasterizer
	{
	public:
		DX11Rasterizer(Graphics& gfx, bool twoSided = true);
		void Bind() override;
		void Bind(IContext& context) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		Graphics& gfx;
	};
}