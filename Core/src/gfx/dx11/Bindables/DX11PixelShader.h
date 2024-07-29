#pragma once
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <string>

namespace tryn::gfx::dx11
{
	class DX11PixelShader : public IPixelShader
	{
	public:
		DX11PixelShader(const Graphics& gfx, const std::string&);
		void Bind() override;
		void Bind(IContext& context) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPS;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		const Graphics& gfx;
	};
}