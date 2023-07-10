#pragma once
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <vector>

struct D3D11_INPUT_ELEMENT_DESC;


namespace tryn::gfx::dx11
{
	class DX11VertexBuffer;
	class DX11VertexShader;
	class DX11InputLayout : public IInputLayout
	{
	public:
		DX11InputLayout(Graphics& gfx , DX11VertexBuffer& vb , DX11VertexShader& vs);
		void Bind() override;

	private:
		Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> buffer;
	};
}