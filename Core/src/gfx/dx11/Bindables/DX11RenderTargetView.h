#pragma once
#include <Core/src/gfx/bindables/RenderTargetView.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

namespace tryn::gfx::dx11
{
	template <RTType Type>
	class DX11RenderTargetView : public IRenderTargetView<Type>
	{
	public:
		DX11RenderTargetView(Graphics& gfx, const spa::DimensionsI dimensions);
		void Bind() override;
		void Bind(class IContext& ctx) override;
	private:
		Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
	};
}