#pragma once
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>

namespace tryn::gfx::dx11
{
	class DX11Rasterizer : public IRasterizer
	{
	public:
		DX11Rasterizer(const Graphics& gfx, bool twoSided = true);
		void Bind() override;
		void Bind(const IContext& context) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		const Graphics& gfx;
	};
}