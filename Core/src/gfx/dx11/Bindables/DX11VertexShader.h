#pragma once
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <core/src/gfx/dx11/Dx11Graphics.h>
#include <string>

namespace tryn::gfx::dx11
{
	class DX11VertexShader : public BaseVertexShader
	{
	public:
		DX11VertexShader(Graphics& gfx , std::wstring path);
		void Bind() override;
		const Microsoft::WRL::ComPtr<ID3DBlob>& GetBlob() const;
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVS;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		Graphics& gfx;
	};
}