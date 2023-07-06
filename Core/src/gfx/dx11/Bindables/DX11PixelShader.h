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
		DX11PixelShader( Graphics& gfx , std::wstring );
		void Bind() override;
	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPS;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		Graphics& gfx;
	};
}